#include "ble_comm.h"
#include "app_error.h"

#include "string.h"
#include "user_flash.h"
#include "ble_adv.h"
#include "ble_init.h"
#include "nrf_soc.h"
#include "driver_init.h"
#include "lis3dh_app.h"





/**************************************
*function    : ble_set_name
*description : if set ,it will updata device name
***************************************/
void set_ble_name(void)
{
	uint32_t err_code;

    ble_gap_conn_sec_mode_t sec_mode;
	
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&sec_mode); //  mo write chara
//  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
	

    uint8_t devname[8]={"BLE APIS"};
	
	// set name
    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)devname,sizeof(devname));
    APP_ERROR_CHECK(err_code);
}



/**************************************
*@function    : get_device_mac
*@description :
***************************************/
ble_gap_addr_t   m_device_addr;
void ble_mac_init(void)
{
	uint8_t temp_mac[6];

	memset(temp_mac,0xff,sizeof(temp_mac));
    sd_ble_gap_addr_get(&m_device_addr);

	//StuHis.mac_addr addr is null
	if(memcmp(StuHis.mac_addr,temp_mac,6)==0)
	{
	   memcpy(StuHis.mac_addr,m_device_addr.addr,6);		
	}
	else
    {		
	   memcpy(m_device_addr.addr,StuHis.mac_addr,6);	
	   sd_ble_gap_addr_set(&m_device_addr);
	}
}

/**************************************
*@function    : get_device_mac
*@description : [data out] 6byte device mac
***************************************/
void set_ble_mac(uint8_t* p_data)
{
	uint32_t err_code;
	uint8_t i;
    sd_ble_gap_addr_get(&m_device_addr);
	
	ble_adv_stop();
	

	for(i=0;i<6;i++)
	{
	   StuHis.mac_addr[i]=p_data[5-i];
	}	
	
	//Because it is a static address type,highest byte 0xC-
	StuHis.mac_addr[5]=0xC0;
	memcpy(m_device_addr.addr,StuHis.mac_addr,6);	
	
	err_code = sd_ble_gap_addr_set(&m_device_addr);
	BLE_RTT("[set_ble_mac] err_code=%x\r\n",err_code);
	
	set_ble_start();
}



/*********************************************************
*function:  task_device_tx_power
*description: set ble tx power
*nrf52832: tx_power={4, 0, -4, -8, -12, -16, -20, -40, -40}
*********************************************************/
void task_ble_tx_power(void)
{
	uint32_t err_code = 0 ;
	static int8_t temp_txPower=0xff;

	
	if(temp_txPower!=StuHis.tx_Power)
	{
		temp_txPower = StuHis.tx_Power;
		
		err_code = sd_ble_gap_tx_power_set(StuHis.tx_Power);
		APP_ERROR_CHECK(err_code);
	}	
}








/****************************************************************
*@function    : get_device_mac
*@description : [data out] 6byte device mac
***************************************/
void ble_disconnect(void)
{
    uint16_t temp_conn_handle = get_m_conn_handle(); 
	
	if(temp_conn_handle!=BLE_CONN_HANDLE_INVALID)
         sd_ble_gap_disconnect(temp_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);

//    BLE_RTT("err:%x\r\n",err);
}


/**********************************************
*function:get_nrf52_chip_temp
*
********************************************/
void get_nrf52_chip_temp(void)
{
	static uint8_t time_cnt=0;
	
	if(++time_cnt<10)   return;
	time_cnt=0;
		
    int32_t temp;                                        // variable to hold temp reading
    (void)sd_temp_get(&temp);                            // get new temperature

	BLE_RTT("[nrf52_chip]  temp=%x\r\n",temp);
	
//  m_tlm.temp[0] = (uint8_t)((temp_new >> 2) & 0xFFUL); // Right-shift by two to remove decimal part
//  m_tlm.temp[1] = (uint8_t)((temp_new << 6) & 0xFFUL); // Left-shift 6 to get fractional part with 0.25 degrees C resolution
}





/*****************************************
*/
//uint32_t nfc_soc_type =0;
void get_device_type(uint8_t *p_data,uint8_t *data_len)
{
    uint8_t tmp = 0,len = 0;

    p_data[0] = 0x10;//nrf52810

    /* 1:????*/
    uint32_t nfc_soc_type = NRF_FICR->INFO.PART;
    if(nfc_soc_type == 0x00052805) 
    {
        p_data[0] = 0x15;
    }
    else if(nfc_soc_type == 0x00052832) 
    {
        p_data[0] = 0x11;
    }
    else if(nfc_soc_type == 0x00052833) 
    {
        p_data[0] = 0x13;
    }

    /* 2:chekc device is have lis3dh sensor*/
    if(get_3dh_sensor_state()==true)
    {
      tmp|=0x01;
    }
    p_data[1] = tmp;
	
	*data_len = 2;
}



