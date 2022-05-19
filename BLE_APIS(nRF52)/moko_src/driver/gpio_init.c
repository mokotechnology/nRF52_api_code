#include "gpio_init.h"
#include "nrf_drv_gpiote.h"
#include "app_timer.h"

#include "ble_gap.h"
#include "nrf_delay.h"
#include "driver_init.h"
#include "led_manage.h"
#include "ble_adv.h"
#include "ble_comm.h"
#include "simple_uart.h"

#include "spi_io.h"
#include "lis3dh_app.h"
#include "stk832x_app.h"

/**************************************************
*function:get_rx_irq_sta
*description : get rx pin  irq sta 
*************************************/
static uint8_t rx_irq_flg=1;

uint8_t get_rx_irq_sta(void)
{
    return rx_irq_flg;
}

void clean_rx_irq(void)
{
    rx_irq_flg=0;
}

/**************************************
*function:key_irq_hand
*description: hanlde pin irq function 
**********************/
uint8_t key_press_flg=0;
void pin_irq_handle(nrf_drv_gpiote_pin_t pin_no, nrf_gpiote_polarity_t action)
{
	if(nrf_gpio_pin_read(KEY_PIN)==0)
	{ 
		key_press_flg=1;
		BLE_RTT("key_irq_hand....\r\n");
	}
	if(nrf_gpio_pin_read(RX_PIN)==0)
	{ 
		rx_irq_flg =1;
		BLE_RTT("UART_Rx_PIN is low\r\n");
	}
    if(nrf_gpio_pin_read(LIS3DH_INT1_PIN)==1)
	{ 
		BLE_RTT("3axis irq enable...\r\n");
	}
}

/**********************************************************************
*function:enable_pin_irq
*description:  enable pin irq 
**************************************************************/
void enable_pin_irq(uint32_t pin,bool enable_flg)
{
    uint32_t err_code;
		
    nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_HITOLO(false);
    config.pull = NRF_GPIO_PIN_PULLUP;

	// key---------init
    if(KEY_PIN == pin)
    {
        config.sense = NRF_GPIOTE_POLARITY_TOGGLE;
        config.hi_accuracy =false;
    }
	else if(RX_PIN==pin)
	{
        config.sense = NRF_GPIOTE_POLARITY_HITOLO;
        config.hi_accuracy =false;	
	}
	if(LIS3DH_INT1_PIN==pin)
	{
	    config.sense = NRF_GPIOTE_POLARITY_TOGGLE;
		config.pull = NRF_GPIO_PIN_NOPULL;
		
        config.hi_accuracy =false;	
	}
		
    if (!nrf_drv_gpiote_is_init())
    {
        err_code = nrf_drv_gpiote_init();
        APP_ERROR_CHECK(err_code);
    }

    if(true == enable_flg)
    {
        err_code = nrf_drv_gpiote_in_init(pin, &config, pin_irq_handle);
        APP_ERROR_CHECK(err_code);
        nrf_drv_gpiote_in_event_enable(pin, true);
    }
    else
    {
        nrf_drv_gpiote_in_event_disable(pin);
        nrf_drv_gpiote_in_uninit(pin);
    }
}





/****************************************
*user_gpio_init
***************************************/
void user_gpio_init(void)
{	
	if(lis3dh_init()==false)
	{
	   nrf_delay_ms(20);
       STK832x_Init();
	}
	nrf_gpio_cfg_input(LIS3DH_INT1_PIN,NRF_GPIO_PIN_NOPULL);
	enable_pin_irq(LIS3DH_INT1_PIN,true);
}





