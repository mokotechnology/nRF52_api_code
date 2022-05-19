#include "uart_commd.h"
#include "simple_uart.h"
#include "string.h"
#include "nrf_delay.h"
#include "user_flash.h"
#include "ble_comm.h"
#include "ble_adv.h"


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
    buf[len++]=0xed;
    buf[len++]=2;
    buf[len++]=cmd;
	
    if(dlen==0)
    {
		buf[len++]=0x01;
        if(datas==0)
        {
            buf[len++]=0xaa;
            buf[len++]=0xaa;
        }
        else if((int)datas==1)
        {
            buf[len++]=0x55;
            buf[len++]=0x55;
        }
        else if((int)datas==2)
        {
            buf[len++]=0x66;
            buf[len++]=0x66;
        }
    }
    else// return data 
    {
        buf[len++]=dlen;
        for(i=0; i<dlen; i++)
        {
            buf[len++]=datas[i];
            cs+=datas[i];
        }
        buf[len++]=cs;
    }
	
    buf[len++]=0xee;
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
				    buf[i]=StuHis.mac_addr[5-i];
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
                if(len==4)
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
				len=0;
				buf[len++]=get_device_type();
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
	    	break;
   }
}
