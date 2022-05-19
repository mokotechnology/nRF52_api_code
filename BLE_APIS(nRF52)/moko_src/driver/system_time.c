#include "system_time.h"
#include "app_timer.h"
#include "system_time.h"

#include "key.h"
#include "sht3x_driver.h"
#include "ble_adv.h"

#include "ble_comm.h"
#include "simple_uart.h"
#include "lis3dh_app.h"
#include "stk832x_app.h"

/********************************************************************/ // variable variable define 
// timer 
APP_TIMER_DEF(system_timer_id);  
static uint8_t system_timer_flg;  

/*******************************************************/
static void system_timer_Hand(void * p_context)
{
	UNUSED_PARAMETER(p_context);
    system_timer_flg = 1;
	
}

void system_timer_init(void)
{
	uint32_t err_code;
	 // Create timers.
    err_code = app_timer_create(&system_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                system_timer_Hand);
	APP_ERROR_CHECK(err_code);
	
	
	system_timer_start();
}

void system_timer_start(void)
{
	ret_code_t err_code;
    err_code = app_timer_start(system_timer_id,  APP_TIMER_TICKS(1000) , NULL);
    APP_ERROR_CHECK(err_code);
}

void system_timer_stop(void)
{
	ret_code_t err_code;
	err_code = app_timer_stop(system_timer_id);
	APP_ERROR_CHECK(err_code);
}



/*******************************************
*task_read_3_aixas
**********************************/
static void task_read_3_axis(void)
{
   if(get_3dh_sta()==1)	
	   task_read_3dh();
   else
	   STK832x_read_data();
}




/**************************************
*system timer 1s
*
************************************/
void task_system_timer(void)
{	
	if(system_timer_flg==0) return;	
	
	   system_timer_flg =0;
	
	task_ble_tx_power();
	
	device_OnOff_manage();
	task_ble_adv_start();
	
	task_check_uart_timeout();
	
	get_nrf52_chip_temp();
	
    task_read_3_axis();
	task_read_sht3x();
	
	
}  







