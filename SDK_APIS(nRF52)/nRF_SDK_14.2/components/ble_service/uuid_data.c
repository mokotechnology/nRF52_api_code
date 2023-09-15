#include "uuid_data.h"
#include <stdint.h>
#include <string.h>
#include "main.h"
#include "library.h"
#include "battery_driver.h"
#include "lis3dh_driver.h"

#include "sdk_macros.h"

#include "system_time.h"





/*********************Set_UUID********************************/   // void Set_UUID(ble_nus_t * p_nus,ble_gatts_evt_write_t *evt)
void Set_UUID(ble_nus_t * p_nus,ble_nus_evt_t *evt)
{
	if((evt->params.rx_data.length) == 16)
	{
		memcpy(p_nus->user_data.UUID, evt->params.rx_data.p_data, 16);
	}
	ble_uuid_update(p_nus, &p_nus->uuid_hand, p_nus->user_data.UUID,16);
}

/*********************set_major********************************/
void set_major(ble_nus_t * p_nus,ble_nus_evt_t *evt)
{
	if((evt->params.rx_data.length) == 2)
	{
		memcpy(p_nus->user_data.Major, evt->params.rx_data.p_data, 2);
	}
	ble_uuid_update(p_nus, &p_nus->major_hand, p_nus->user_data.Major,2);
}


/************************************************************
*function   : send_to_App
*para: @cmd :
*      @len :user data len,no include cmd,
*      @data:data
**************************************************************/
void send_to_App(ble_nus_t * p_nus,uint8_t cmd,uint8_t len,uint8_t *data)
{
	uint8_t app_data[20] = {0x00};
	
	memset(app_data,0,20);
	
	app_data[0] = 0xEB;
	app_data[1] = cmd;
	app_data[2] = 0x00;
	app_data[3] = len;

	memcpy(&app_data[4],data,len);
	nus_send(p_nus,app_data,len+4,2);
}

/*********************************************************
*function name: updata_HandleValu
*Description  : 更新uuid通道句柄值
*
*********************************************************/
void updata_HandleValu(ble_nus_t * p_nus)
{
	uint32_t err_code;
	err_code = ble_uuid_update(p_nus, &p_nus->uuid_hand, p_nus->user_data.UUID,16);
	
	APP_ERROR_CHECK(err_code);
	
	ble_uuid_update(p_nus, &p_nus->major_hand, p_nus->user_data.Major,2);


}








