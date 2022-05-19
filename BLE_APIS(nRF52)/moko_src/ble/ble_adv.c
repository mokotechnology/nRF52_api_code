#include "ble_adv.h"
#include "ble_advdata.h"
#include "stdint.h"

#include "app_error.h"
#include "ble_comm.h"
#include "driver_init.h"
#include "user_flash.h"
#include "ble_init.h"





/*******************************************************************/         
uint8_t beacon_infor[21] = /**< Information advertised by the Beacon. */
{
    0xE2,0xC5,0x6D,0xB5,0xDF,0xFB,0x48,0xD2,0xB0,0x60,0xD0,0xF5,0xA7,0x10,0x96,0xE0, // uuid
    0X00,0x00,// major
    0x00,0x00,// mainor
    0xC3      // rssi
};

/***********************************************************
*@function :iBeacon_adv_data_set
*@describe : set ibeacon data  in ble adv 
*******************************************/
static  uint8_t m_adv_data[31]={0};
static  uint8_t adv_data_len;

static void iBeacon_adv_pack_set(void)
{
    adv_data_len=0;	
	m_adv_data[adv_data_len++]= 0x02;//Len
    m_adv_data[adv_data_len++]= 0x01;//type
    m_adv_data[adv_data_len++]= 0x06;
	
	// ibeacon data 
	m_adv_data[adv_data_len++]= sizeof(beacon_infor)+1+4;//len=sizeof(beacon_infor)+len(type)+4
    m_adv_data[adv_data_len++]= 0xFF; //type
	
    m_adv_data[adv_data_len++]= 0x4C;
    m_adv_data[adv_data_len++]= 0x00;
    m_adv_data[adv_data_len++]= 0x02;
    m_adv_data[adv_data_len++]= 0x15;
	
	memcpy(m_adv_data+adv_data_len,beacon_infor,sizeof(beacon_infor));
	  adv_data_len+=sizeof(beacon_infor);
}



/***********************************************************
*@function :iBeacon_adv_data_set
*@describe : set ibeacon response packet data  
              in ble adv = Raw data to be placed in scan response packet
*******************************************/
static  uint8_t m_rsp_data[31]={0};
static  uint8_t rsp_data_len;

static void iBeacon_rsp_pack_set(void)
{
    rsp_data_len=0;	
	m_rsp_data[rsp_data_len++]= 0x02;//Len
    m_rsp_data[rsp_data_len++]= 0x0A;//type	
    m_rsp_data[rsp_data_len++]= 0x00;//TX power vale
	
	//rsp_data
	m_rsp_data[rsp_data_len++]= strlen("BLE APIS")+1;
    m_rsp_data[rsp_data_len++]= 0x09; //type
	
	memcpy(m_rsp_data+rsp_data_len,"BLE APIS",strlen("BLE APIS"));
	  rsp_data_len+=strlen("BLE APIS");
}

/***********************************************************
*@function :adv_data_padding
*@describe : Raw data  = adv packet + scan response packet
*******************************************/
static void adv_data_padding(void)
{
    uint32_t    err_code;
	
	
	iBeacon_adv_pack_set();
	err_code = sd_ble_gap_adv_data_set(m_adv_data,adv_data_len,NULL,0);
		
	iBeacon_rsp_pack_set();
    err_code = sd_ble_gap_adv_data_set(m_adv_data,adv_data_len,m_rsp_data,rsp_data_len);

	if(err_code!=NRF_SUCCESS)
	{
		BLE_RTT("[adv_data_padding] err_code=%x \r\n",err_code);
	    APP_ERROR_CHECK(err_code);
	}
}

/***********************************************************
*@function : adv_params_config
*@describe : set ble adv para
*******************************************/
static 	ble_gap_adv_params_t adv_params;
static void adv_params_config(void)
{
	memset(&adv_params, 0, sizeof(ble_gap_adv_params_t));
	
    adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;   //BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;

    adv_params.p_peer_addr = NULL; 
    adv_params.fp          = BLE_GAP_ADV_FP_ANY;

    adv_params.interval    = MSEC_TO_UNITS(StuHis.adv_interval, UNIT_0_625_MS);//BLE_GAP_ADV_INTERVAL_MIN
    adv_params.timeout     =  0;

}

/*********************************************
*  ble_adv_stop
***********************/
void ble_adv_stop(void)
{
	uint32_t    err_code;
	err_code = sd_ble_gap_adv_stop();
	if(err_code!=NRF_SUCCESS)
	{
	    BLE_RTT("[ble_adv_stop]===>err=%d\r\n",err_code);
	}
//    APP_ERROR_CHECK(err_code);
}


/*********************************************
*  ble_adv_start
***********************/
static void ble_adv_start(void)
{
    uint32_t    err_code;

	adv_data_padding();	
	adv_params_config();
	
    err_code = sd_ble_gap_adv_start(&adv_params, BLE_CONN_CFG_TAG_DEFAULT);
	
	if(err_code!=NRF_SUCCESS)
	{
		BLE_RTT("[ble_adv_start]  err=0x%x\r\n",err_code);
		APP_ERROR_CHECK(err_code);	
	}
}


/*********************************************************
*function:ble_adv_start_mode
*
*************************************/
static uint8_t ble_start_flg=0;
void set_ble_start(void)
{
	ble_start_flg=1;
}

void  task_ble_adv_start(void)
{
	
	if(is_ble_connect()==true)    return;
	
	if(ble_start_flg==0)   return;
       ble_start_flg=0;
	
	ble_adv_stop();
    ble_adv_start();
}







