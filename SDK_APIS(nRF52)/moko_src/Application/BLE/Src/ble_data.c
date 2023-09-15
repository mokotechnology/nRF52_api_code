#include "ble_data.h"
#include "periph_service.h"
#include "user_flash.h"
#include "ble_adv.h"
#include "bsp.h"


/************************************************************************
*function: nus_send_cmd
*[data in]:cmd :commd  
**********************************************/
uint32_t nus_send_cmd(uint8_t cmd,uint8_t *p_data,uint8_t lenth)
{
    uint32_t err;
    uint8_t buf[50];
    uint16_t len=0,i;
	
	
    buf[len++]=0xeb;
    buf[len++]=cmd;
    buf[len++]=0;
    buf[len++]=lenth;
	
    if(lenth)
    {
        for(i=0; i<lenth; i++)
            buf[len++]=p_data[i];
    }
	
    Periph_service_send(FEX0_CHAR,buf,len);
//    BLE_RTT("result:%x",err);
    return err;
}


/*****************************************************************
*@brief ble_commd_analyze 
*[data in]:p_data :app data in   len :data len
*[head] [cmd]  [sn]  [data len]  data
*0xea     10    00      01       01
*********************************/
void  ble_commd_analyze(uint8_t *p_data,uint8_t len)
{
    uint8_t cmd,dataslen;
    uint16_t temp_i16;
    uint8_t buf[20];

    if(p_data[0]!=0xea ||(p_data[3]+4)!=len)
    {
        nus_send_cmd(0x0d,0,0);
        return;
    }

	cmd= p_data[1];
    dataslen = p_data[3];	

    switch(cmd)
    {
		case 0x01://read adv interval
			//check 
		    if((len>4)||p_data[2]!=0||p_data[3]!=0)
            {
				nus_send_cmd(0x0d,0,0);
				return;
			} 
			buf[0]=StuHis.adv_interval>>8;//hig Byte
			buf[1]=StuHis.adv_interval;//low Byte
						
			nus_send_cmd(cmd,buf,2);
			break;		
		case 0x02://set adv interval		
		    temp_i16 =p_data[4];
		    temp_i16 =(temp_i16<<8)|p_data[5];
		
			if(2==dataslen)//set
			{
                StuHis.adv_interval=temp_i16;
				save_user_flash();
				
				set_ble_start();
				nus_send_cmd(cmd,0,0);
			}
			else
			{
				nus_send_cmd(0x0d,0,0);
			}
			break;
		
		case 0x03://read tx power
			if((len>4)||p_data[2]!=0||p_data[3]!=0)
            {
				nus_send_cmd(0x0d,0,0);
				return;
			} 
			buf[0]=StuHis.tx_Power;//low Byte
			nus_send_cmd(cmd,buf,1);
			break;		
		case 0x04://set tx power
			if(1==dataslen)//set
			{
                StuHis.tx_Power=p_data[4];
				
				save_user_flash();
				nus_send_cmd(cmd,0,0);
			}
			else
			{
				nus_send_cmd(0x0d,0,0);
			}
			break;
		default:break;
	}
}
















