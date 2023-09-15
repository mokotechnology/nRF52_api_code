//#ifndef LIB_DRIVER_IIC

//#define LIB_DRIVER_IIC

//#include <Driver_includes.h>

//#endif
#include "vl53l0x_iic.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"



//��������˫�����ݡ�������
#define SDA_CLR          NRF_GPIO->OUTCLR = 1<<TWI_SDA
#define SDA_SET          NRF_GPIO->OUTSET = 1<<TWI_SDA
#define SDA_RD          (NRF_GPIO->IN>>TWI_SDA)&1

//��������ʱ���źš�������
#define SCL_CLR		 NRF_GPIO->OUTCLR = 1<<TWI_SCL
#define SCL_SET		 NRF_GPIO->OUTSET = 1<<TWI_SCL


#define XSHUT_CLR		 NRF_GPIO->OUTCLR = 1<<XSHUT_IO
#define XSHUT_SET		 NRF_GPIO->OUTSET = 1<<XSHUT_IO



//״̬
#define STATUS_OK       0x00
#define STATUS_FAIL     0x01


//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


//SDA����Ϊ����
void SDA_IN(void)
{

	nrf_gpio_cfg_input(TWI_SDA, NRF_GPIO_PIN_PULLUP);

}	

//SDA����Ϊ���
void SDA_OUT(void) 
{

	nrf_gpio_cfg_output(TWI_SDA);

}


//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	SDA_SET;	  	  
	SCL_SET;
	//nrf_delay_us(2);
 	SDA_CLR;//START:when CLK is high,DATA change form high to low 
	//nrf_delay_us(2);
	SCL_CLR;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	SCL_CLR;
	SDA_CLR;//STOP:when CLK is high DATA change form low to high
 	nrf_delay_us(2);
	SCL_SET; 
	SDA_SET;//����I2C���߽����ź�
	//nrf_delay_us(2);							   	
}


//��ʼ��IIC
void IIC_Init(void)
{			
	nrf_gpio_cfg_output(TWI_SDA);
	nrf_gpio_cfg_output(TWI_SCL);
    nrf_gpio_cfg_output(XSHUT_IO);

    XSHUT_SET;

    nrf_delay_us(500);

}

void shut_down_vl53l0x(void)
{
    XSHUT_CLR;
}


//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
unsigned char IIC_Wait_Ack(void)
{
	unsigned char ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	SDA_SET;
	//nrf_delay_us(1);	   
	SCL_SET;
	//nrf_delay_us(1);	 
	while(SDA_RD)
	{
		ucErrTime++;
		if(ucErrTime>200)
		{
			IIC_Stop();
			return 1;
		}
	}
	SCL_CLR;//ʱ�����0 	   
	return 0;  
} 

//����ACKӦ��
void IIC_Ack(void)
{
	SCL_CLR;
	SDA_OUT();
	SDA_CLR;
	//nrf_delay_us(1);
	SCL_SET;
	nrf_delay_us(1);
	SCL_CLR;
}

//������ACKӦ��		    
void IIC_NAck(void)
{
	SCL_CLR;
	SDA_OUT();
	SDA_SET;
	//nrf_delay_us(1);
	SCL_SET;
	nrf_delay_us(1);
	SCL_CLR;
}

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(unsigned char txd)
{                        
    unsigned char t;   
	SDA_OUT(); 
	//nrf_delay_us(1); 	
    SCL_CLR;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        if((txd&0x80)>>7)
		{
			SDA_SET;
		}
		else
		{
			SDA_CLR;
		}
        txd<<=1; 	  
		//nrf_delay_us(1);   //��TEA5767��������ʱ���Ǳ����
		SCL_SET;
		nrf_delay_us(1);
		SCL_CLR;	
		//nrf_delay_us(1);
    }	 
} 	

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
unsigned char IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
	//nrf_delay_us(1); 
    for(i=0;i<8;i++ )
	{
        SCL_CLR; 
        nrf_delay_us(1);
		SCL_SET;
        receive<<=1;
        if(SDA_RD)
		{
			receive++;  
		}			
		nrf_delay_us(1);
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}



//IIC��һ���ֽ�����
unsigned char IIC_Read_1Byte(unsigned char SlaveAddress, unsigned char REG_Address,unsigned char *REG_data)
{
	IIC_Start();
	IIC_Send_Byte(SlaveAddress);//��д����
	if(IIC_Wait_Ack())
	{
		 IIC_Stop();//�ͷ�����
		 return 1;//ûӦ�����˳�
	}		
	IIC_Send_Byte(REG_Address);
	IIC_Wait_Ack();
	IIC_Start(); 
	IIC_Send_Byte(SlaveAddress|0x01);//��������
	IIC_Wait_Ack();
	*REG_data = IIC_Read_Byte(0);
	IIC_Stop();

	return 0;
}

//IICдn�ֽ�����
unsigned char IIC_Write_nByte(unsigned char SlaveAddress, unsigned char REG_Address,unsigned short len, unsigned char *buf)
{
	IIC_Start();
	IIC_Send_Byte(SlaveAddress);//��д����
	if(IIC_Wait_Ack()) 
	{
		IIC_Stop();//�ͷ�����
		return 1;//ûӦ�����˳�
	}
	IIC_Send_Byte(REG_Address);
	IIC_Wait_Ack();

	while(len--)
	{
		IIC_Send_Byte(*buf++);//����buff������
		IIC_Wait_Ack();	
	}
	IIC_Stop();//�ͷ�����

	return 0;
	
}

//IIC��n�ֽ�����
unsigned char IIC_Read_nByte(unsigned char SlaveAddress, unsigned char REG_Address,unsigned short len,unsigned char *buf)
{
	IIC_Start();
	IIC_Send_Byte(SlaveAddress);//��д����
	if(IIC_Wait_Ack()) 
	{
		IIC_Stop();//�ͷ�����
		return 1;//ûӦ�����˳�
	}
	IIC_Send_Byte(REG_Address);
	IIC_Wait_Ack();
        IIC_Stop();

	IIC_Start();
	IIC_Send_Byte(SlaveAddress|0x01);//��������
	IIC_Wait_Ack();
	while(len)
	{
		if(len==1)
		{
			*buf = IIC_Read_Byte(0);
		}
		else
		{
			*buf = IIC_Read_Byte(1);
		}
		buf++;
		len--;
	}
	IIC_Stop();//�ͷ�����

	return 0;
	
}

//VL53l0x д�������
//address:��ַ
//index:ƫ�Ƶ�ַ
//pdata:����ָ��
//count:���� ���65535
unsigned char VL53l0x_write_multi(unsigned char address, unsigned char index,unsigned char *pdata,unsigned short count)
{
	unsigned char status = STATUS_OK;

	if(IIC_Write_nByte(address,index,count,pdata))
	{
	   status  = STATUS_FAIL;

	}

	return status;
}


//VL53l0x ���������
//address:��ַ
//index:ƫ�Ƶ�ַ
//pdata:����ָ��
//count:���� ���65535
unsigned char VL53l0x_read_multi(unsigned char address,unsigned char index,unsigned char *pdata,unsigned short count)
{
	unsigned char status = STATUS_OK;

	if(IIC_Read_nByte(address,index,count,pdata))
	{
	  status  = STATUS_FAIL;
	}

	return status;
}

//VL53l0x д1������(���ֽ�)
//address:��ַ
//index:ƫ�Ƶ�ַ
//data:����(8λ)
unsigned char VL53l0x_write_byte(unsigned char address,unsigned char index,unsigned char data)
{
	unsigned char status = STATUS_OK;

	status = VL53l0x_write_multi(address,index,&data,1);

	return status;
}

//VL53l0x д1������(˫�ֽ�)
//address:��ַ
//index:ƫ�Ƶ�ַ
//data:����(16λ)
unsigned char VL53l0x_write_word(unsigned char address,unsigned char index,unsigned short data)
{
	unsigned char status = STATUS_OK;
	
	unsigned char buffer[2];
	
	//��16λ���ݲ�ֳ�8λ
	buffer[0] = (unsigned char)(data>>8);//�߰�λ
	buffer[1] = (unsigned char)(data&0xff);//�Ͱ�λ
	
	if(index%2==1)
	{  
		//����ͨ�Ų��ܴ���Է�2�ֽڶ���Ĵ������ֽ�
		status = VL53l0x_write_multi(address,index,&buffer[0],1);
		status = VL53l0x_write_multi(address,index,&buffer[0],1);
	}else
	{
		status = VL53l0x_write_multi(address,index,buffer,2);
	}
	
	return status;
}

//VL53l0x д1������(���ֽ�)
//address:��ַ
//index:ƫ�Ƶ�ַ
//data:����(32λ)
unsigned char VL53l0x_write_dword(unsigned char address,unsigned char index,unsigned int data)
{
	
    unsigned char status = STATUS_OK;

    unsigned char buffer[4];	
	
	//��32λ���ݲ�ֳ�8λ
	buffer[0] = (unsigned char)(data>>24);
	buffer[1] = (unsigned char)((data&0xff0000)>>16);
	buffer[2] = (unsigned char)((data&0xff00)>>8);
	buffer[3] = (unsigned char)(data&0xff);
	
	status = VL53l0x_write_multi(address,index,buffer,4);
	
	return status;
	
}


//VL53l0x ��1������(���ֽ�)
//address:��ַ
//index:ƫ�Ƶ�ַ
//data:����(8λ)
unsigned char VL53l0x_read_byte(unsigned char address,unsigned char index,unsigned char *pdata)
{
	unsigned char status = STATUS_OK;
	 
	status = VL53l0x_read_multi(address,index,pdata,1);
	
	return status;
	 
}

//VL53l0x ��������(˫�ֽ�)
//address:��ַ
//index:ƫ�Ƶ�ַ
//data:����(16λ)
unsigned char VL53l0x_read_word(unsigned char address,unsigned char index,unsigned short *pdata)
{
	unsigned char status = STATUS_OK;
	
	unsigned char buffer[2];
	
	status = VL53l0x_read_multi(address,index,buffer,2);
	
	*pdata = ((unsigned short)buffer[0]<<8)+(unsigned short)buffer[1];
	
	return status;
	
}

//VL53l0x ��1������(���ֽ�)
//address:��ַ
//index:ƫ�Ƶ�ַ
//data:����(32λ)
unsigned char VL53l0x_read_dword(unsigned char address,unsigned char index,unsigned int *pdata)
{
	unsigned char status = STATUS_OK;
	
	unsigned char buffer[4];
	
	status = VL53l0x_read_multi(address,index,buffer,4);
	
	*pdata = ((unsigned int)buffer[0]<<24)+((unsigned int)buffer[1]<<16)+((unsigned int)buffer[2]<<8)+((unsigned int)buffer[3]);
	
	return status;
	
}





//IICдn�ֽ�����
unsigned char IIC_Write_nByte_t(unsigned char SlaveAddress, unsigned short len, unsigned char *buf)
{
	IIC_Start();
	IIC_Send_Byte(SlaveAddress);//��д����
	if(IIC_Wait_Ack()) 
	{
		IIC_Stop();//�ͷ�����
		return 1;//ûӦ�����˳�
	}
	IIC_Send_Byte(*buf++);
	IIC_Wait_Ack();
	len--;
	while(len--)
	{
		IIC_Send_Byte(*buf++);//����buff������
		IIC_Wait_Ack();	
	}
	IIC_Stop();//�ͷ�����

	return 0;
	
}
//IIC��n�ֽ�����
unsigned char IIC_Read_nByte_t(unsigned char SlaveAddress,unsigned short len,unsigned char *buf)
{
//	IIC_Start();
//	IIC_Send_Byte(SlaveAddress);//��д����
//	if(IIC_Wait_Ack())
//	{
//		IIC_Stop();//�ͷ�����
//		return 1;//ûӦ�����˳�
//	}
//	IIC_Send_Byte(REG_Address);
//	IIC_Wait_Ack();

	IIC_Start();
	IIC_Send_Byte(SlaveAddress|0x01);//��������
	IIC_Wait_Ack();
	while(len)
	{
		if(len==1)
		{
			*buf = IIC_Read_Byte(0);
		}
		else
		{
			*buf = IIC_Read_Byte(1);
		}
		buf++;
		len--;
	}
	IIC_Stop();//�ͷ�����

	return 0;
	
}
//VL53l0x д�������
//address:��ַ
//index:ƫ�Ƶ�ַ
//pdata:����ָ��
//count:���� ���65535
unsigned char VL53l0x_write_multi_t(unsigned char address, unsigned char *pdata,unsigned short count)
{
	unsigned char status = STATUS_OK;

	if(IIC_Write_nByte_t(address,count,pdata))
	{
	   status  = STATUS_FAIL;

	}

	return status;
}
//VL53l0x ���������
//address:��ַ
//index:ƫ�Ƶ�ַ
//pdata:����ָ��
//count:���� ���65535
unsigned char VL53l0x_read_multi_t(unsigned char address,unsigned char *pdata,unsigned short count)
{
	unsigned char status = STATUS_OK;

	if(IIC_Read_nByte_t(address,count,pdata))
	{
	  status  = STATUS_FAIL;
	}

	return status;
}























