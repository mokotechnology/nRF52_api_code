#include "sdk_common.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "ble_init.h"
#include "periph_service.h"
#include "nrf_delay.h"

#include "utility.h"
#include "ble_data.h"
#include "bsp.h"


/******************************************************/
ble_feex_t    m_feex_servive;

/****************************************************************************************************************************
*@brief Function for slave service add
*@ para:[1]p_nus:Service event types      [2]p_nus_init:Event handler to be called  [3]uuid:uuid valu
*       [4]uuid_index:uuid index          [5]write_flg: write                       [6]read_flg: read      [7]notifty_flg: notifty
*       [8]p_desc:enable descri function  [9]p_src: descri valu                     [10]data_len: descri data len
***************************************************************************************************************************/
static uint32_t feex_char_add(ble_feex_t * p_nus, const ble_feex_init_t * p_nus_init, 
				              uint16_t uuid,uint8_t uuid_index, bool write_flg, bool read_flg,
						      bool notifty_flg,uint8_t *p_desc ,uint8_t *p_src,uint8_t data_len)
{
	ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

	// cccd_md set
    memset(&cccd_md, 0, sizeof(cccd_md));
    if (notifty_flg)
    {
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
//    cccd_md.rd_auth = 0;
//    cccd_md.wr_auth = 0;
//    cccd_md.vlen    = 1;
    }
		
    memset(&char_md, 0, sizeof(char_md));
	// set char_md
    if (read_flg)
    {
        char_md.char_props.read          = 1;
    }
    if (write_flg)
    {
        char_md.char_props.write         = 1;
        char_md.char_props.write_wo_resp = 1;
    }
	
    char_md.char_props.notify = notifty_flg;

    char_md.p_char_user_desc         = p_desc;
    char_md.p_char_pf                = NULL;
    char_md.p_user_desc_md           = NULL;
    char_md.p_cccd_md                = NULL;
    char_md.p_sccd_md                = NULL;	
	// set uuid 
	ble_uuid.type = p_nus->uuid_type;
    ble_uuid.uuid = uuid;

	// att
	memset(&attr_md, 0, sizeof(attr_md));
    if (read_flg)
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
	
    if (read_flg)
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;
	
	//att char value
	memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = data_len;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_NUS_MAX_DATA_LEN;
	attr_char_value.p_value   = p_src;

    return sd_ble_gatts_characteristic_add(p_nus->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_nus->FFFX[uuid_index]);
}
/********************************************************************************
*[1] app or centeral enbale feex ccfg
*
****************************************/
static bool enable_feex_ccfg(ble_feex_t * p_feex, ble_evt_t const* p_ble_evt)
{
	uint8_t i;
 
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

//	for(i=0;i<UUID_NUMBERS;i++)
	{
//		BLE_RTT("enable fee hande=%d,cccd_handle=%d\r\n",p_evt_write->handle,p_feex->FFFX[i].cccd_handle);
		
		if((p_evt_write->handle == p_feex->FFFX[0].cccd_handle)&&(p_evt_write->len == 2))
		{
			if (ble_srv_is_notification_enabled(p_evt_write->data))
			{
				p_feex->is_notification_enabled[0] = true;
			}
			else
			{
				p_feex->is_notification_enabled[0] = false;				
			}
			BLE_RTT("enable fee%x=%d\r\n",i,p_feex->is_notification_enabled[0]);
		} 
		
		if((p_evt_write->handle == p_feex->FFFX[1].cccd_handle)&&(p_evt_write->len == 2))
		{
			if (ble_srv_is_notification_enabled(p_evt_write->data))
			{
				p_feex->is_notification_enabled[1] = true;	
			}
			else
			{
				p_feex->is_notification_enabled[1] = false;				
			}
			BLE_RTT("enable fee%x=%d\r\n",i,p_feex->is_notification_enabled[1]);
		} 		
	}
    return true;
}


/**********************************************
*
* on_feex_write 写入事件   form master(centern)
**************************************************/
static void on_feex_write(ble_feex_t * p_feex, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    ble_nus_evt_rx_data_t  rx_evt;
	
	rx_evt.p_data = p_evt_write->data;
    rx_evt.length = p_evt_write->len;
	
	if(p_evt_write->handle == p_feex->FFFX[0].value_handle)
    {
        ble_commd_analyze((uint8_t *)rx_evt.p_data ,rx_evt.length);
    } 
	else
	{
          //LOG_SERVICE("err write\r\n");
	}
}



/**********************************************************
*
* periph_service handel evt
*********************************************************/
bool c_hvn_tx_complete=false;
void periph_service_on_ble_evt(ble_feex_t * p_nus, ble_evt_t const * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:			
            break;
		case BLE_GAP_EVT_DISCONNECTED:
			break;
        case BLE_GATTS_EVT_WRITE:
			enable_feex_ccfg(p_nus,p_ble_evt);
            on_feex_write(p_nus,p_ble_evt);
            break;
		
		case BLE_GATTS_EVT_HVN_TX_COMPLETE:
			c_hvn_tx_complete =true;
			break;

        default:
			break;
    }
}


/****************************************************************
*  ble_feex_init  16bit uuid
***********************************/
uint32_t ble_feex_init(ble_feex_t * p_nus, ble_feex_init_t const * p_nus_init)
{
	uint16_t primy_serv_uuid  =0xFEE0;
	
    ret_code_t            err_code;
    ble_uuid_t            ble_uuid;
    ble_uuid128_t         nus_base_uuid = NUS_BASE_UUID;
	
    VERIFY_PARAM_NOT_NULL(p_nus);
    VERIFY_PARAM_NOT_NULL(p_nus_init);

    // Initialize the service structure.
    p_nus->data_handler = p_nus_init->data_handler;

    err_code = sd_ble_uuid_vs_add(&nus_base_uuid, &p_nus->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_nus->uuid_type;
    ble_uuid.uuid = primy_serv_uuid;

    // Add the service.
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_nus->service_handle);
    VERIFY_SUCCESS(err_code);
	
	
   /*W  write
	*N  Notify*/
	err_code =feex_char_add(p_nus, p_nus_init,primy_serv_uuid+1,0,true,false,true,(uint8_t *)"W_N",(uint8_t *)"abb",3);
	err_code =feex_char_add(p_nus, p_nus_init,primy_serv_uuid+2,1,false,false,true,(uint8_t *)"N",(uint8_t *)"abab",4);

    if (err_code != NRF_SUCCESS)   return err_code;

    return NRF_SUCCESS;
}


/*****************************************************
* ble_service_send
* @para：uuid_index:  0 ------2
******************************************************/
uint32_t Periph_service_send(uint8_t uuid_index,uint8_t *p_data,uint16_t lenth)
{
	
    ret_code_t                 err_code;
    ble_gatts_hvx_params_t     hvx_params;
	
    ble_feex_t * p_nus=&m_feex_servive;

    if ((p_nus->conn_handle == BLE_CONN_HANDLE_INVALID) 
		||( !(p_nus->is_notification_enabled[uuid_index])))
    {
		BLE_RTT("NRF_ERROR_NOT_FOUND conn_handle=%d,,,,notif=%d\r\n",
		         p_nus->conn_handle,p_nus->is_notification_enabled[uuid_index]);
		
        return NRF_ERROR_NOT_FOUND;
    }

    if((lenth > BLE_NUS_MAX_DATA_LEN)||(lenth==0))
    {
		BLE_RTT("BLE_NUS_MAX_DATA_LEN err len=%d\r\n",lenth);
        return NRF_ERROR_INVALID_PARAM;
    }

    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = p_nus->FFFX[uuid_index].value_handle;
    hvx_params.p_data = p_data;
    hvx_params.p_len  = &lenth;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;

	uint16_t i;
	for(i=0; i<3; i++)
    {
		err_code=sd_ble_gatts_hvx(p_nus->conn_handle, &hvx_params);
        if(NRF_SUCCESS==err_code)
        {
            BLE_RTT("slave send success:handle:%d uuid:%d  is_data:%d len:%d\r\n", hvx_params.handle,16,1,lenth);
            return NRF_SUCCESS;
        }
        delay_ms(1000);
    }
    return err_code;
}


/***********************************
*  user service init
**********************************/
void user_service_init(void)
{
	uint32_t err_code;
    ble_feex_init_t     feex_init;
	
    // Initialize NUS.
    memset(&feex_init, 0, sizeof(feex_init));

    err_code = ble_feex_init(&m_feex_servive, &feex_init);
    APP_ERROR_CHECK(err_code);
}





