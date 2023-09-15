#include "uart_commd.h"
#include "simple_uart.h"
#include "string.h"
#include "nrf_delay.h"
#include "user_flash.h"
#include "ble_comm.h"
#include "ble_adv.h"
#include "app_uart.h"
#include "bsp.h"

/*********************************************
*function :uart_send_cmd
*[data in]: cmd:data cmd 
*           datas:user data 
*           dlen:data len 
**********************************/
void uart_send_cmd(uint8_t cmd,uint8_t*datas,uint8_t dlen)
{
#define MAX_LEN 50
    uint8_t buf[MAX_LEN],len=0,i,cs=0;
	
    buf[len++] = 0xed;
    buf[len++] = 2;
    buf[len++] = cmd;
	
    if(dlen==0)
    {
		buf[len++] = 0x01;
        if(datas ==  0)
        {
            buf[len++] = 0xaa;
            buf[len++] = 0xaa;
        }
        else if((int)datas==1)
        {
            buf[len++] = 0x55;
            buf[len++] = 0x55;
        }
        else if((int)datas==2)
        {
            buf[len++] = 0x66;
            buf[len++] = 0x66;
        }
    }
    else// return data 
    {
        buf[len++] = dlen;
        for(i=0; i<dlen; i++)
        {
            buf[len++] = datas[i];
            cs += datas[i];
        }
        buf[len++] = cs;
    }
	
    buf[len++] = 0xee;
    uart_send(buf,len);
}



/************************************************
* analyze_uart_data
*
****************************************/
void analyze_uart_data(uint8_t cmd,uint8_t*datas,uint8_t len)
{
	uint8_t buf[50];
	uint8_t i;
   switch(cmd)
   {
        case 1://read and set mac  cmd
            if(0==len)
            {
                for(i=0;i<6;i++)
				{
				    buf[i] = StuHis.mac_addr[5-i];
				}
				uart_send_cmd(cmd,buf,6);
            }
            else
            {
                if(len == 6)//set mac
                {			
					set_ble_mac(datas);
                    uart_send_cmd(cmd,0,0);
					ask_save_factory();
                }
                else
                {
                    uart_send_cmd(cmd,(uint8_t*)1,0);
                }
            }
            break; 			
        case 7:  // produce_time
            if(0==len)
            {
                uart_send_cmd(cmd,StuHis.produce_time,4);
            }
            else
            {
                if(len  == 4)
                {
                    memcpy(StuHis.produce_time,datas,len);
                    uart_send_cmd(cmd,0,0);
					ask_save_factory();
                }
                else
                {
                    uart_send_cmd(cmd,(uint8_t*)1,0);
                }
            }
            break;				
	 case 0x0f:
			if(0==len)
			{
				len = 0;
				
				get_device_type(buf,&len);
				uart_send_cmd(cmd,buf,len);
			}
			else
			{
				uart_send_cmd(cmd,(uint8_t*)1,0);
			}
			break;
		case 0x10://clsoe uart
			uart_send_cmd(cmd,0,0);
			nrf_delay_ms(50);
			start_uart(false);
			ask_save_factory();
			break;		

		case 0x34://entern device sleep
			uart_send_cmd(cmd,0,0);
			nrf_delay_ms(50);
			start_uart(false);
			ble_adv_stop();
		   
		    /*if have other periph ,it must be to close */
		    /**/
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









