#include "sht3x_driver.h"
#include "iic_io.h"

#include "utility.h"
#include "in_flash_manage.h"
#include "driver_init.h"
#include "nrf_delay.h"



/******************************************************************/
uint8_t  TemHum_buf[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
int Temp_valu = 0;
int Hum_valu = 0;

static uint8_t  is_have_sht3x = 1;


/************************************************
*function:  sht30_writeReg
*para:[data in] reg_addr:sht30 reg addr 
*               value :set reg valu
*****************************************/
void sht30_write_reg(uint8_t reg_addr,uint8_t value)
{
	uint8_t temp_reg[2] = {0};
	temp_reg[0] = reg_addr;
	temp_reg[1] = value;
	
	hrs_iic_transfer(SHT30_SALVE_ADDR, temp_reg, 2, TWI_ISSUE_STOP);
}

/*****************************************************************************
*function name:  sht30_read_reg
*description  :  read sht30 reg data 
*[data in]:  
*hig_addr: reg hig addr;
*low_reg:  reg low addr;
*  
*[data out]p_data: reg data 
           len   :read data len
*******************************************************************************/
void sht30_read_reg(uint8_t hig_addr,uint8_t low_reg,
	                uint8_t len,uint8_t *p_data)
{
	uint8_t temp_reg[2] = {0};
	temp_reg[0] = hig_addr;
	temp_reg[1] = low_reg;	
		
	hrs_iic_transfer(SHT30_SALVE_ADDR, temp_reg, 2, TWI_DONT_ISSUE_STOP);
	hrs_iic_transfer(SHT30_SALVE_ADDR|0x01, p_data, len, TWI_ISSUE_STOP);
}

/************************************************
*function:  crc_high_first
*para:[data in] ptr: form data 
*               len :data len
*****************************************/
unsigned char crc_high_first(unsigned char *ptr, unsigned char len)
{
    unsigned char i;
    unsigned char crc=0xff; /* ?????crc? */

    while(len--)
    {
        crc ^= *ptr++;  /* ?????????????,????????? */
        for (i=8; i>0; --i)   /* ???????????????crc?? */
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc = (crc << 1);
        }
    }
    return (crc);
}

/************************************************
*function:sht30_Init
*description:
*****************************************/
void sht3x_Init(void)
{
    uint8_t i;
    uint8_t  temp_data1[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
	uint8_t  temp_data2[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
	
	
	nrf_gpio_cfg_output(SHT30_PWR_PIN);
    SHT30_PWR_ON();
    
  	hrs_iic_init(SHT30_SDA_PIN,SHT30_SCL_PIN);
    delay_ms(10);
	
    sht30_write_reg(0x30,0xA2);
	sht30_write_reg(0x30,0x93);//Stop Periodic Data Acquisition Mode

    for(i=0;i<10;i++)
    {	
        task_read_sht3x();
		
        if((memcmp(TemHum_buf,temp_data1,6)!= 0)&&
			(memcmp(TemHum_buf,temp_data2,6)!=0))
        {
            is_have_sht3x = 1;// existen tem_Hum sonser
			
			BLE_RTT("[SHT3X] have_sht3x......\r\n");
            return;
        }
        delay_ms(50);
    }
	is_have_sht3x=0;
}


void  get_sht3x_valu(uint8_t *p_data)
{
p_data[0]=Temp_valu;
p_data[1]=Hum_valu;
}


/*******************************************************************************
* Function Name  : read_tempHum_valu
* Description    : 读取温湿度传感
* Input          :
* Output         : None
* Return         : None
*******************************************************************************/
void task_read_sht3x(void)
{
	
	static uint8_t time_cnt=0;
	
    if(is_have_sht3x==0) return;
	
	if(++time_cnt<5)     return ;
	time_cnt=0;
	
	
	sht30_read_reg(0x2C,0x10,0x10,TemHum_buf);
	
    if(TemHum_buf[2]!=crc_high_first(TemHum_buf,2) 
		    || TemHum_buf[5]!=crc_high_first(TemHum_buf+3,2)) 
	{
		BLE_RTT("[SHT3X] read data crc is err\r\n");
		return ;
	}

    //int Temp_valu
    Temp_valu = (TemHum_buf[0]<<8|TemHum_buf[1]);
    Temp_valu = ((100 * Temp_valu * 175) / 65535 - 45 * 100)/10;

    Hum_valu = (TemHum_buf[3]<<8|TemHum_buf[4]);
    Hum_valu = (100 * Hum_valu * 100)/65535/10;
	
    BLE_RTT("[SHT3X] t:%d H:%d\r\n",Temp_valu,Hum_valu);
}
