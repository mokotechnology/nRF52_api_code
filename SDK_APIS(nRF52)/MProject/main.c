#include "nrf_dfu.h"
#include "ble_adv.h"
#include "ble_init.h"
#include "driver_init.h"
#include "system_time.h"
#include "wdt.h"
#include "simple_uart.h"
#include "led_manage.h"

#include "user_flash.h"
#include "uart_commd.h"
#include "bsp.h"
#include "vl53lx_app.h"


/**************************************************
* @function: assert_nrf_callback err 
********************************************/
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}


#define  SOFTWARE_VER 1
/*******************************/
int main(void)
{
	
	BLE_RTT("-----current sys ver is %s ,last modify time is %s %s ------\r\n",SOFTWARE_VER,__DATE__,__TIME__);
	BLE_RTT("---chip ram size is %x\r\n",NRF_FICR->INFO.RAM*1024);	
	
	//dfu boot para addr
	boot_to_new_appilacation(0,0);	
	
	Platform_driver_init();	
	ble_Init();
	
	//dc-dc enable----
	sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
	
	BLE_RTT("device start.........\r\n");
	
    for (;;)
    {
        
		led_timer_manage();
				
        task_process_uart_data();
		task_uart_manage();
			
		//down dfu file 
	    task_dfu_upfile();			
		task_system_timer();

        calculate_vl53lx_distance();
		
		task_save_flash();
		Feed_WacthDog();
        Power_manager();
    }
}









