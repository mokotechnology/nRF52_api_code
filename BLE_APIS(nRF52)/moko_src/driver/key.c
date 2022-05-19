#include "key.h"

#include "ble_adv.h"
#include "driver_init.h"
#include "led_manage.h"
#include "ble_comm.h"




extern uint8_t key_press_flg;

/***********************************************/
static uint8_t device_sta= 1;

uint8_t get_device_sta(void)
{
    return device_sta;
}

/**********************************************
*@funtion     :key_ledInit
*@description :init led and key; key use gpiote driver
**********************************************/
void key_Init(void)
{			
   enable_pin_irq(KEY_PIN,true);
}

/**********************************************
*@funtion     :check_key_press
*@description :check key is short press sucess
**********************************************/
static uint8_t short_press_flg=1;
void check_key_press(void)
{
	static uint8_t time_cnt =0;
	
	if(key_press_flg==0)  
	{
		time_cnt =0;
		return;
	}
	
	if(nrf_gpio_pin_read(KEY_PIN)==0)
	{
		if(++time_cnt==2)
		{
			short_press_flg=1;
		}	
	}
	else
	{
		key_press_flg=0;
	}	
}

/*********************************************
*stop_adv_mode
***********************************************/
static uint8_t stop_adv_flg =0;
static void stop_adv_mode(void)
{
	if(stop_adv_flg==0)  return;
       stop_adv_flg=0;
      
      ble_adv_stop();	
}


/*******************************************
*@function: device_OnOff_manage
*
****************************/
void device_OnOff_manage(void)
{
	static uint8_t sta=0;
	
	check_key_press();
	
	stop_adv_mode();
	
	if(short_press_flg==0)  return;	
	   short_press_flg=0;
	
	switch(sta)
	{
		case 0://device on sta
			BLE_RTT("device on...... \r\n");
		    led_ctrl(50,50,20);
		
		    set_ble_start();
		    device_sta=1;
		    sta=1;
			break;		
		case 1://device off sta
			BLE_RTT("device off...... \r\n");
		
		    ble_disconnect();
		    led_ctrl(2000,0,1);
		
		    stop_adv_mode();
		    device_sta=0;
		    sta=0;
			break;
		
		default:
			break;
	}
}
















