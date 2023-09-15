#include "led_manage.h"
#include "app_timer.h"
#include "key.h"
#include "nrf_gpio.h"
#include "gpio_init.h"
#include "bsp.h"


static uint16_t led_on_ms, led_off_ms,led_times;
static uint8_t led_sta=0,led_timer_tick;



/***********************************************
*@function: led_init
*
*********************************************/
void led_init(void)
{
	nrf_gpio_cfg_output(LED_PIN);	
//	nrf_gpio_pin_set(LED_PIN);
	
	led_timer_init();
}


/*****************************************************/
APP_TIMER_DEF(m_led_timer);
static void led_timer_timeout(void * p_context)
{
    led_timer_tick++;
	
//  BLE_RTT("key_time:%d\r\n",key_pressed_ms);
}

/********************************************
*@init_led_timer
*************************/
void led_timer_init(void)
{
    ret_code_t err_code;

    err_code = app_timer_create(&m_led_timer,
                                APP_TIMER_MODE_REPEATED,
                                led_timer_timeout);
    APP_ERROR_CHECK(err_code);
}

/********************************************
*@start_timer
*************************/
static void start_timer(bool start_flg,uint8_t p_timer,uint16_t timer_ms)
{
    if(start_flg)
    {
		app_timer_start(m_led_timer,APP_TIMER_TICKS(timer_ms),NULL);
//      BLE_RTT("timer on:%d id:%02x\r\n",p_timer,time_id);
    }
    else
    {
      app_timer_stop(m_led_timer);
	  BLE_RTT(" led timer stop\r\n");
    }
}

/*********************************************************************
*
*led_ctrl
******************************/
void led_ctrl(uint16_t on_ms,uint16_t off_ms,uint16_t times)
{
    led_on_ms=on_ms/50;
    led_off_ms=off_ms/50;
    led_times=times;
    led_sta=0;
    start_timer(true,LED_TIMER,50);
    led_timer_tick=0;
}

/****************************************
*led_timer_manage
* 放到 mian 函数的while 循环
*************************************/
void led_timer_manage(void)
{
    if(led_timer_tick==0)    return;
	
    led_timer_tick--;
    static uint16_t endtick=0;
	
    switch(led_sta)
    {
        case 0:
            if(led_times)
            {
                endtick=led_on_ms;			
                LED_ON();			
                if(endtick==0)
                    endtick=1;
                led_sta++;
            }
            break;
        case 1:
            endtick--;
            if(0==endtick)
            {
                endtick=led_off_ms;
                if(endtick==0)
                    endtick=1;
                LED_OFF();
                led_sta++;
            }
            break;
        case 2:
            endtick--;
            if(0==endtick)
            {
                led_times--;
                if(0==led_times)
                {
                    led_sta=0;
                    start_timer(false,LED_TIMER,50);
                    LED_OFF();
                    break;
                }
                else
                {
                    endtick=led_on_ms;
                    LED_ON();
                    led_sta=1;
                }
            }
            break;
    }
}
