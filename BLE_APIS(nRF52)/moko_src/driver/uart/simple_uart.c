#include "stdint.h"
#include "app_uart.h"
#include "simple_uart.h"
#include "uart_commd.h"
#include "nrf_gpio.h"
#include "nrf_uarte.h"
#include "driver_init.h"
#include "key.h"
#include "simple_uart.h"





extern uint32_t app_uart_init(const app_uart_comm_params_t * p_comm_params,
                       app_uart_buffers_t *           p_buffers,
                       app_uart_event_handler_t       error_handler,
                       app_irq_priority_t             irq_priority);


/***********************************************************************
*function :   uart_error_handle
*description: hanlde uart error callback
******************************************/
static void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
//        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
//        APP_ERROR_HANDLER(p_event->data.error_code);
	}
}

	
/***********************************************************
*function:simple_uart_init
*
*************************************************/
void simple_uart_init(void)
{
    uint32_t err_code;

    const app_uart_comm_params_t comm_params[1] =
    {
      {
          RX_PIN,
          TX_PIN,
          0,
          0,
          APP_UART_FLOW_CONTROL_DISABLED,
          false,
          NRF_UART_BAUDRATE_9600
      },
  } ;
      
    APP_UART_FIFO_INIT(&comm_params[0],
                         UART_RX_BUF_SIZE,
                         UART_TX_BUF_SIZE,
                         uart_error_handle,
                         APP_IRQ_PRIORITY_LOWEST,
                         err_code);

    APP_ERROR_CHECK(err_code);
}

/***********************************************************
*function:simple_uart_uinit
*
*************************************************/
void simple_uart_uinit(void)
{
	app_uart_close();
}

/***********************************************************
*function:uart_send
*
*************************************************/
void uart_send(uint8_t *str,uint16_t len)
{
	uint16_t i,counter;
	for(i=0;i<len;i++)
	{
		counter=0;
	    while(NRF_SUCCESS!=app_uart_put(str[i]) && counter++<16000);
	}
}

/*******************************************************************************
*function : start_uart(bool set_flg)
*desription:start uart and disable rx pin irq, 
             or close uart and enable rx pin irq
*******************************************/
void start_uart(bool set_flg)
{
	static uint8_t temp_flg =0xff;
	
	if(set_flg==temp_flg)   return;
	    temp_flg= set_flg;

	if(set_flg==true)
	{
		enable_pin_irq(RX_PIN,false);
		
		BLE_RTT("[UART] open ....\r\n");
		simple_uart_init();			
	}
	else
	{
		simple_uart_uinit();

		BLE_RTT("[UART] close ....\r\n");
		nrf_gpio_cfg_input(RX_PIN,NRF_GPIO_PIN_PULLUP);
		nrf_gpio_cfg_input(TX_PIN,NRF_GPIO_PIN_PULLUP);
		
		enable_pin_irq(RX_PIN,true);
	}
}

/******************************************************************
*function :  set_uart_time_out_flg
*description: timeout 30s ,uart will close,after wait RX pin irq wake
**********************************************/
static  uint8_t uart_timeout_flg =0;
void task_check_uart_timeout(void)
{	
	static uint8_t time_cnt=0;
	
	if(++time_cnt>=30)//30s
	{
	    uart_timeout_flg=1;
		time_cnt =0;
	}
}

static void clean_uart_timeout(void)
{
   uart_timeout_flg =0;
}

/****************************************************
*function :   task_uart_manage
*description: manege uart open and close
***************************************************/
void task_uart_manage(void)
{
	static uint8_t sta=1;	
	switch(sta)
	{
		case 1:// uart will open
			if(get_rx_irq_sta())
			{
				start_uart(true);
								
				clean_uart_timeout();				
				clean_rx_irq();
				sta=2;
			}
    		break;	
		case 2:// uart will close	
            if(uart_timeout_flg)
			{	
                uart_timeout_flg =0;				
				start_uart(false);
				sta =1;				
			}
			break;
	}
}

/**************************************************************
*function:    task_process_uart_data
*description: get uart data and check data
*************************************/
void task_process_uart_data(void)
{
#define TIME_OUT_MS  (100/10)
#define BUF_MAX_LEN 100
   
    uint8_t dataout;
    static uint8_t sta=0,buf[BUF_MAX_LEN+3],len,left,cs;
    
	  while(NRF_SUCCESS== app_uart_get(&dataout))
      {
		BLE_RTT("%x ",dataout);
        switch(sta)
        {
            case 0:
				if(dataout==0xed)
				{
                    len=0;
					buf[len++]=dataout;
				    sta++;  
           	    }	
                break;				
            case 1:
				if(dataout==1)
				{
					sta++;
					buf[len++]=dataout;
				}
				else
				{
					sta=0;  
				}                
                break;				
             case 2:
				sta++;
				buf[len++]=dataout;             
                break;
            case 3:
				buf[len++]=dataout;
				if(dataout==0)
				{
					left=1;
					sta=4;
				}
				else
				{
				   left=dataout+2;
					cs=0;
					sta=5;
				}				
				if(left >BUF_MAX_LEN)
				{
				    sta=0;
				}
				break; 				
            case 4:
				if(left==1)
				{
					left=0;
					if(0!=dataout)
					{
					   sta=0; 
					}
				}
				else
				{					
					if(dataout==0xee)//read
					{					
						//buf  01 cmd 00 
                        analyze_uart_data(buf[2],0,0);
					}
					sta=0; 
				}
                break;				
            case 5:
				buf[len++]=dataout;
				if(left>2)
				cs+=dataout;
						
				left--; // สฃำเ---
				if(0==left)//write
				{
					sta=0;
					//buf  02 cmd len datas cs ee
					if(0xee == dataout && cs== buf[len-2])
					{      
						 analyze_uart_data(buf[2],&buf[4],buf[3]); // buf[3] =len
					}   
                    else if(cs!= buf[len-2]) // check data cs error ?
                    {
					    uart_send_cmd(buf[2],(uint8_t*)2,0);
					}					
				}   
                break;
				
			default:
				break;
        }           
      }     
     return;
}







