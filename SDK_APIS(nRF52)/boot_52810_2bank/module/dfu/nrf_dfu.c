#include "nrf_dfu.h"
#include "bsp.h"
#include "nordic_ble.h"
#include "ble_fifo.h"
#include "nrf_sdm.h"	
	
	
#define DFU_SD 1
#define DFU_BL 2
#define DFU_APP 4

#define DFU_REV_MAJOR                        0x00                                                    /** DFU Major revision number to be exposed. */
#define DFU_REV_MINOR                        0x08                                                    /** DFU Minor revision number to be exposed. */
#define DFU_REVISION                         ((DFU_REV_MAJOR << 8) | DFU_REV_MINOR)                  /** DFU Revision number to be exposed. Combined of major and minor versions. */

#ifdef DFU_USE_EXT_FLASH

uint16_t  dfu_crc(uint8_t *buf,uint32_t len);
#endif

typedef enum
{
    PKT_TYPE_INVALID,                                                                                /**< Invalid packet type. Used for initialization purpose.*/
    PKT_TYPE_START,                                                                                  /**< Start packet.*/
    PKT_TYPE_INIT,                                                                                   /**< Init packet.*/
    PKT_TYPE_FIRMWARE_DATA                                                                           /**< Firmware data packet.*/
} pkt_type_t;

bool dfu_que_out(uint8_t*dataout,uint8_t* len);
bool dfu_que_in(uint8_t*datain,uint8_t len);
void dfu_que_init(uint8_t* pbuf,uint8_t size);

static  ble_dfu_t            m_dfu;                                                                   /**< Structure used to identify the Device Firmware Update service. */
static pkt_type_t m_pkt_type=PKT_TYPE_INVALID;
static uint8_t  dfu_update_mode;
static uint32_t app_size=0;
static uint32_t app_crc;
#define DFU_FIFI_SIZE  256
static bool m_pkt_rcpt_notif_enabled = true;
static uint32_t m_pkt_notif_target,m_pkt_notif_target_cnt,m_num_of_firmware_bytes_rcvd;
uint16_t crc16_compute_dfu(uint8_t const * p_data, uint32_t size, uint16_t const * p_crc);


bool EraseBvkFlash(uint32_t size)
{
    uint32_t num=0,i,addr;

	if(size<=APP_BVK_SIZE)
    {

        num=size/PAGE_SIZE;
        if(size%PAGE_SIZE)
        {
            num++;
        }
        addr=APP_BVK_ADDR/PAGE_SIZE;
//        LOG_DEBUG_PRINTF("EraseBvkFlash,num=%d\r\n",num);
        for(i=0; i<num; i++)
        {
            Flash_Erase_Page(addr+i);
        }
    }
    return true;
}

/*step1 erase*/
////////////////////////////////////////
void dfu_prepare_space(void)//================================================================================================
{
    EraseBvkFlash(app_size>20*1024 ? app_size:APP_SIZE);
    DFU_RTT("----erse:%x\r\n",app_size);
}
///////////////////////////////////////

/*step2 write*/
////////////////////////////////////////////////////
static bool write_success=false;
void call_by_sys(uint32_t sys_evt,void * p_context)
{
    if(NRF_EVT_FLASH_OPERATION_SUCCESS==sys_evt)
        write_success=true;
}

void dfu_write_flash(uint32_t off_set,uint32_t *buf,uint32_t size)
{
    uint16_t i;
    write_success=false;

    sd_flash_write((uint32_t *)(APP_BVK_ADDR+off_set), buf,size);
    for(i=0; i<10000; i++)
    {
        delay_us(100);
        if(write_success==true)
            return;
    }
    DFU_RTT("write err ---\r\n");

}
/////////////////////////////////////////////////


bool IsRightBoard(char* boardbuf)
{

    bool OkFlg=true;
    if(0!=memcmp("cjy",boardbuf+0,3))
        OkFlg=false;
    return OkFlg;

}

bool check_is_right_firmware(void)
{
    return IsRightBoard((char*)(APP_BVK_ADDR+1024));
}

void check_crc(uint16_t *calc_crc,uint16_t* crcout,uint8_t *rev_buf)
{
    *calc_crc = crc16_compute_dfu((uint8_t *)APP_BVK_ADDR, app_size, NULL);
	*crcout = *calc_crc;
//    *crcout=CalcCrc16(0xffff,(const uint8_t *)APP_BVK_ADDR,app_size);

}


/*step 3 dfu task*/
////////////////////////////////////////////////////

void check_out_data(uint32_t rev_bytes);
//void RevRfDataRecall2(uint8_t * datain);


void CleanUpflg_Reboot(void)
{
	uint32_t err_code;
	uint8_t buf[20];
	delay_ms(500);
	err_code= sd_softdevice_disable();//softdevice_handler_sd_disable();
	APP_ERROR_CHECK(err_code);
	ble_conn_params_stop();
	delay_ms(1000);
	
	memset(buf,0,12);
	boot_to_new_appilacation(buf,12);
	
	delay_ms(1000);
	NVIC_SystemReset();
}
void task_dfu_upfile(void)
{

#define TIME_OUT_NO_DATA        (60000/500)
#define FIFO_SIZE   20
    uint32_t dfu_buf1[5+DFU_FIFI_SIZE/4];
    uint8_t buf_f[FIFO_SIZE*21];
    uint32_t time_out_no_data=0;
    uint32_t ticktime;
    bool new_data,is_full;
    uint16_t calc_crc,crcout,write_len,i;
    uint8_t pdata[20];
    uint8_t len=0;
    uint32_t buf_len;
    uint8_t *buf;
    uint8_t cmp_version=0;
    uint32_t ble_rev_bytes=0;


    if(m_pkt_type==PKT_TYPE_INVALID) return;
    app_size=0;
    DFU_RTT("to dfu task------\r\n");
    time_out_no_data=0;

//    StartHeatRate(false);
    ticktime=NRF_RTC1->COUNTER;
    time_out_no_data=0;
    while(0==app_size)
    {
//        power_manage();
        V_FeedWdog();

        if(get_time_escape_ms(NRF_RTC1->COUNTER,ticktime)>=500)
        {
            ticktime=NRF_RTC1->COUNTER;
            if(time_out_no_data++>=(40))
            {
                DFU_RTT("time out-------->\r\n");
                goto END_DFU;
            }
        }
    }
    time_out_no_data=0;
//    dfu_prepare_space();/*step1 erase*/
    ble_dfu_response_send(&m_dfu,
                          BLE_DFU_START_PROCEDURE,
                          BLE_DFU_RESP_VAL_SUCCESS);

//	fileOffset=0;
    buf_len=0;
    buf=(uint8_t*)dfu_buf1;

    cmp_version=0;

    dfu_que_init(buf_f,FIFO_SIZE);

    is_full=false;


    ticktime=NRF_RTC1->COUNTER;
//    SetHighSpeed();
    while(1)
    {
        V_FeedWdog();
        if( m_dfu.conn_handle == BLE_CONN_HANDLE_INVALID)
        {
            DFU_RTT("dfu dis connect-------->\r\n");
            goto END_DFU;
        }

        if(get_time_escape_ms(NRF_RTC1->COUNTER,ticktime)>=500)
        {
            ticktime=NRF_RTC1->COUNTER;
            if(time_out_no_data++>=TIME_OUT_NO_DATA)
            {
                DFU_RTT("time out-------->\r\n");
                goto END_DFU;
            }
        }

        len=0;
        new_data=dfu_que_out(pdata,&len);
        if((m_num_of_firmware_bytes_rcvd+buf_len)>=app_size)
        {
            is_full=true;
        }
        if(true == new_data)
        {
        LED_TOG();
        }
        if((new_data==false)&&(is_full==false))
        {
            continue;
        }
        
        

        time_out_no_data=0;
        if(len)
        {
            memcpy(buf+buf_len,pdata,len);
            buf_len+=len;
            ble_rev_bytes+=len;
        }

        check_out_data(ble_rev_bytes);

        if( (buf_len<DFU_FIFI_SIZE)&&(is_full==false))
        {
            continue;
        }




        if(buf_len>=DFU_FIFI_SIZE)
        {
            write_len=DFU_FIFI_SIZE;
            buf_len=buf_len-write_len;
        } else
        {
            write_len=buf_len;
            buf_len=buf_len-write_len;
        }

        if(write_len==0)
        {
            continue;
        }

        dfu_write_flash(m_num_of_firmware_bytes_rcvd,(uint32_t*)buf,write_len/4);


        m_num_of_firmware_bytes_rcvd+=write_len;
        if(buf_len)
        {
            for(i=0; i<buf_len; i++)
            {
                buf[i]=buf[write_len+i];
            }
        }


        if((m_num_of_firmware_bytes_rcvd>(1024+16))&&(0==cmp_version))
        {
            cmp_version=1;

            if(true!=check_is_right_firmware())
            {
                goto END_DFU;
            }
        }

        if(m_num_of_firmware_bytes_rcvd<app_size) continue;


        check_crc(&calc_crc,&crcout,(uint8_t *)dfu_buf1);
        if(calc_crc!=app_crc)
        {
            ble_dfu_response_send(&m_dfu,
                                  BLE_DFU_RECEIVE_APP_PROCEDURE,
                                  BLE_DFU_RESP_VAL_CRC_ERROR);
            goto END_DFU;
        }
        ble_dfu_response_send(&m_dfu,
                              BLE_DFU_RECEIVE_APP_PROCEDURE,
                              BLE_DFU_RESP_VAL_SUCCESS);

//        extern uint8_t IapFlg[12];
//        IapFlg[0]=0xaa;
//        IapFlg[1]=0x55;
//        IapFlg[2]=app_size>>24;
//        IapFlg[3]=app_size>>16;
//        IapFlg[4]=app_size>>8;
//        IapFlg[5]=app_size;
//        IapFlg[6]=0;//version
//        IapFlg[7]=0;//version
//        IapFlg[8]=crcout>>8;
//        IapFlg[9]=crcout>>0;
        delay_ms(500);
        LED_ON();
        sd_ble_gap_disconnect(m_dfu.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        m_dfu.conn_handle = BLE_CONN_HANDLE_INVALID;
//        delay_ms(500);
//        WriteFlgBootSys(false);
        
		CleanUpflg_Reboot();


    }

END_DFU:
//    MotorOn(1,1,9);
    BleQueueAllOut();
//    SemEmpty();
//    SendSem(RESULT_UI);
    m_pkt_type=PKT_TYPE_INVALID;
    LED_OFF();
//    TaskUpFile();
    return;

}


uint8_t cmp_version=0;
void init_update(uint8_t mode);
void RevRfDataRecall2(uint8_t * datain)
{

}




#ifdef DFU_PRINTF
uint32_t test_times = 0;
#endif




void on_dfu_pkt_write(ble_dfu_t * p_dfu, ble_dfu_evt_t * p_evt)
{

    uint32_t length = p_evt->evt.ble_dfu_pkt_write.len;
    uint8_t * p_length_data = p_evt->evt.ble_dfu_pkt_write.p_data;
    // The peer has written to the DFU Packet characteristic. Depending on the value of
    // the current value of the DFU Control Point, the appropriate action is taken.
    switch (m_pkt_type)
    {
    case PKT_TYPE_START://DFU_STATE_PREPARING  DFU_STATE_RDY
        app_size = uint32_decode(p_length_data + 8);
        DFU_RTT("app size:%d,%d\r\n",app_size,get_time_escape_ms(NRF_RTC1->COUNTER,test_times));

        // The peer has written a start packet to the DFU Packet characteristic.
//            start_data_process(p_dfu, p_evt);
        break;

    case PKT_TYPE_INIT:
        app_crc=*((uint16_t *)((p_length_data + length-2)));
        DFU_RTT("appcrc:%x\r\n",app_crc);
        break;

    case PKT_TYPE_FIRMWARE_DATA:

        dfu_que_in(p_length_data,length);

        break;

    default:
        // It is not possible to find out what packet it is. Ignore. There is no
        // mechanism to notify the DFU Controller about this error condition.
        break;
    }
}




static void on_dfu_evt(ble_dfu_t * p_dfu, ble_dfu_evt_t * p_evt);
static void service_error_handler(uint32_t nrf_error);
uint32_t dfu_transport_init(void)
{
    uint32_t       err_code;
    ble_dfu_init_t dfu_init_obj;

    // Initialize the Device Firmware Update Service.
    memset(&dfu_init_obj, 0, sizeof(dfu_init_obj));

    dfu_init_obj.revision      = DFU_REVISION;
    dfu_init_obj.evt_handler   = on_dfu_evt;
    dfu_init_obj.error_handler = service_error_handler;

    err_code = ble_dfu_init(&m_dfu, &dfu_init_obj);

    DFU_RTT("dfu init:%x\r\n",err_code);
    APP_ERROR_CHECK(err_code);

    return err_code;
}


void dfu_transport_callby_irq(ble_evt_t * p_ble_evt)
{
    ble_dfu_on_ble_evt(&m_dfu, p_ble_evt);
}


/**@brief     Function for handling Service errors.
 *
 * @details   A pointer to this function will be passed to the DFU service which may need to inform
 *            the application about an error.
 *
 * @param[in] nrf_error Error code containing information about what went wrong.
 */

static void service_error_handler(uint32_t nrf_error)
{
    DFU_RTT("DFU err:%x\r\n",nrf_error);
    APP_ERROR_CHECK(nrf_error);
}

/**@brief     Function for the Device Firmware Update Service event handler.
 *
 * @details   This function will be called for all Device Firmware Update Service events which
 *            are passed to the application.
 *
 * @param[in] p_dfu     Device Firmware Update Service structure.
 * @param[in] p_evt     Event received from the Device Firmware Update Service.
 */

static void on_dfu_evt(ble_dfu_t * p_dfu, ble_dfu_evt_t * p_evt)
{
    uint32_t           err_code;
//    ble_dfu_resp_val_t resp_val;


//		uint32_t i;
//	 uint8_t * p_length_data = p_evt->evt.ble_dfu_pkt_write.p_data;

//		DFU_RTT("\r\n\r\ndata:%d\r\n",p_evt->evt.ble_dfu_pkt_write.len);
//		for(i=0;i<p_evt->evt.ble_dfu_pkt_write.len;i++)
//		{
//		DFU_RTT("%02x",p_length_data[i]);
//		}
//		DFU_RTT("\r\n");
//
//	DFU_RTT("0DFU type:%x\r\n",p_evt->ble_dfu_evt_type);
    switch (p_evt->ble_dfu_evt_type)
    {
    case BLE_DFU_VALIDATE:
        DFU_RTT("1VALIDATE %x\r\n",p_evt->ble_dfu_evt_type);
//            err_code = dfu_image_validate();

//            // Translate the err_code returned by the above function to DFU Response Value.
//            resp_val = nrf_err_code_translate(err_code, BLE_DFU_VALIDATE_PROCEDURE);

        err_code = ble_dfu_response_send(p_dfu, BLE_DFU_VALIDATE_PROCEDURE, BLE_DFU_RESP_VAL_SUCCESS);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_DFU_ACTIVATE_N_RESET:
        DFU_RTT("1N_RESET %x\r\n",p_evt->ble_dfu_evt_type);

//            err_code = dfu_transport_close();
//            APP_ERROR_CHECK(err_code);

//            // With the S110 Flash API it is safe to initiate the activate before connection is
//            // fully closed.
//            err_code = dfu_image_activate();
//            if (err_code != NRF_SUCCESS)
//            {
//                dfu_reset();
//            }
//            break;

    case BLE_DFU_SYS_RESET:

        DFU_RTT("1S_RESET %x\r\n",p_evt->ble_dfu_evt_type);
//			sd_ble_gap_disconnect(p_dfu->conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
//			p_dfu->conn_handle = BLE_CONN_HANDLE_INVALID;
        //  sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
//            err_code = dfu_transport_close();
//            APP_ERROR_CHECK(err_code);

//            dfu_reset();
        break;

    case BLE_DFU_START:

        init_update((uint8_t)p_evt->evt.ble_dfu_pkt_write.p_data[0]);
//		delay_ms(10);
#ifdef DFU_PRINTF
        test_times = NRF_RTC1->COUNTER;
#endif
        DFU_RTT("1DS %x\r\n",p_evt->ble_dfu_evt_type);
        break;

    case BLE_DFU_RECEIVE_INIT_DATA:
        DFU_RTT("ini_da  type :-----------init%x\r\n",p_evt->ble_dfu_evt_type);
        m_pkt_type = PKT_TYPE_INIT;
        if ((uint8_t)p_evt->evt.ble_dfu_pkt_write.p_data[0] == 1)
        {


//				delay_ms(10);
            err_code = ble_dfu_response_send(p_dfu, BLE_DFU_INIT_PROCEDURE, BLE_DFU_RESP_VAL_SUCCESS);
            APP_ERROR_CHECK(err_code);
        }
        break;

    case BLE_DFU_RECEIVE_APP_DATA:
        DFU_RTT("app_da  ------------------type data %x\r\n",p_evt->ble_dfu_evt_type);
        m_pkt_type = PKT_TYPE_FIRMWARE_DATA;

//			m_pkt_rcpt_notif_enabled = true;
//			m_pkt_notif_target       = 12;
//			m_pkt_notif_target_cnt   = 12;
        break;

    case BLE_DFU_PACKET_WRITE:
//			DFU_RTT("PW%x\r\n",p_evt->ble_dfu_evt_type);
        on_dfu_pkt_write(p_dfu, p_evt);
        break;

    case BLE_DFU_PKT_RCPT_NOTIF_ENABLED:

        m_pkt_rcpt_notif_enabled = true;
        m_pkt_notif_target       = p_evt->evt.pkt_rcpt_notif_req.num_of_pkts;
        m_pkt_notif_target_cnt   = p_evt->evt.pkt_rcpt_notif_req.num_of_pkts;

        DFU_RTT("no_en%x  %d %d\r\n",p_evt->ble_dfu_evt_type,m_pkt_notif_target,m_pkt_notif_target_cnt);
        break;

    case BLE_DFU_PKT_RCPT_NOTIF_DISABLED:
        DFU_RTT("no_di%x\r\n",p_evt->ble_dfu_evt_type);
        m_pkt_rcpt_notif_enabled = false;
        m_pkt_notif_target       = 0;
        break;

    case BLE_DFU_BYTES_RECEIVED_SEND:
        DFU_RTT("r_s%x\r\n",p_evt->ble_dfu_evt_type);
//            err_code = ble_dfu_bytes_rcvd_report(p_dfu, m_num_of_firmware_bytes_rcvd);
//            APP_ERROR_CHECK(err_code);
        break;

    default:
        DFU_RTT("1DFU type:default %x\r\n",p_evt->ble_dfu_evt_type);
        // Unsupported event received from DFU Service. Ignore.
        break;
    }
}


#define UNIT_QUE  21
bool dfuQueueflg=false;
uint8_t que_in=100,que_out=0,que_size;
uint8_t *que_buf;

void dfu_que_init(uint8_t *pbuf,uint8_t size)
{
    que_in=0;
    que_out=0;
    dfuQueueflg = false;
    que_size=size;
    que_buf=pbuf;

}

bool dfu_que_in(uint8_t*datain,uint8_t len)
{
    uint8_t* pdata_n;
//	DFU_RTT("%02x%02x--%d\r\n",datain[0],datain[1],len);

    if ((que_in + 1) % que_size == que_out) // 队列满
    {
        DFU_RTT("%d:%d-------------fifo full \r\n",que_in,que_out);
    }

    pdata_n=que_buf+que_in*UNIT_QUE;
    memcpy(pdata_n+1,datain,len);
    pdata_n[0]=len;
    que_in++;
    que_in%=que_size;
    dfuQueueflg=true;
    return true;


}


bool dfu_que_out(uint8_t*dataout,uint8_t* len)
{
    uint8_t* pdata_n;
    if(dfuQueueflg!=true)
    {

        return false;
    }
    else
    {
//		DFU_RTT("1");
        pdata_n=que_buf+que_out*UNIT_QUE;
        *len=pdata_n[0];
        memcpy(dataout,pdata_n+1,*len);

        que_out++;
        que_out %= que_size;
        if(que_out==que_in)
        {
            dfuQueueflg = false;
        }
        return true;
    }
}






void init_update(uint8_t mode)
{
    m_pkt_type    = PKT_TYPE_START;
    dfu_update_mode=mode;
    app_crc=0;
    app_size=10000;
//	fileOffset=0;///////////////////////////////////
    m_num_of_firmware_bytes_rcvd=0;
    DFU_RTT(" dfu_update_mode ------------type start:%x\r\n",dfu_update_mode);

    if(dfu_update_mode!=DFU_APP)
    {
        DFU_RTT("err,update mode is not app \r\n");
        return;
    }

}

uint16_t crc16_compute_dfu(uint8_t const * p_data, uint32_t size, uint16_t const * p_crc)
{
    uint16_t crc = (p_crc == NULL) ? 0xFFFF : *p_crc;

    for (uint32_t i = 0; i < size; i++)
    {
        crc  = (uint8_t)(crc >> 8) | (crc << 8);
        crc ^= p_data[i];
        crc ^= (uint8_t)(crc & 0xFF) >> 4;
        crc ^= (crc << 8) << 4;
        crc ^= ((crc & 0xFF) << 4) << 1;
    }

    return crc;
}
#ifdef DFU_USE_EXT_FLASH

uint16_t  dfu_crc(uint8_t *buf,uint32_t len)
{
    uint16_t tmpcrc=0xffff;

    uint32_t num,last,j,addr;


    last=len;
    num=app_size/len;
    if(app_size%len)
    {
        num++;
        last=app_size%len;
    }
    addr=EX_APP_BVK_ADDR;//EX_UI_ADDR;///EX_UI_ADDR;
    for(j=0; j<num; j++)
    {
        if(j==(num-1))
        {
            len=last;
        }
        SPI_FLASH_ReadCont(buf,addr,len);
        addr+=len;
        tmpcrc = crc16_compute_dfu((uint8_t *)buf, len, &tmpcrc);
    }

    return tmpcrc;

}
#endif


void check_out_data(uint32_t rev_bytes)
{
    if (m_pkt_rcpt_notif_enabled)
    {
//			DFU_RTT("cnt:%d\r\n",m_pkt_notif_target_cnt);
        m_pkt_notif_target_cnt--;

        if (m_pkt_notif_target_cnt == 0)
        {
            ble_dfu_pkts_rcpt_notify(&m_dfu, rev_bytes);
            // Reset the counter for the number of firmware packets.
            m_pkt_notif_target_cnt = m_pkt_notif_target;
        }
    }
}
