
#ifndef vl53l0x_iix_H
#define vl53l0x_iix_H


#define TWI_SDA      13

#define TWI_SCL      14

#define XSHUT_IO     12

#define VL53_VDD     10
//*--------------------------------------------------------------------------------------
//*���Ŷ���
//*--------------------------------------------------------------------------------------
#endif 
unsigned char IIC_Write_nByte_t(unsigned char SlaveAddress, unsigned short len, unsigned char *buf);

unsigned char IIC_Read_nByte_t(unsigned char SlaveAddress,unsigned short len,unsigned char *buf);
#ifdef LIB_DRIVER_IIC 	   		 

//��ʼ��IIC��IO��
void IIC_Init(void);

//SDA����Ϊ����
void SDA_IN(void);

//SDA����Ϊ���
void SDA_OUT(void);

//����IIC��ʼ�ź�
void IIC_Start(void);				

//����IICֹͣ�ź�
void IIC_Stop(void);	  			

//IIC����һ���ֽ�
void IIC_Send_Byte(unsigned char txd);			

//IIC��ȡһ���ֽ�
unsigned char IIC_Read_Byte(unsigned char ack);

//IIC�ȴ�ACK�ź�
unsigned char IIC_Wait_Ack(void); 				

//IIC����ACK�ź�
void IIC_Ack(void);					

//IIC������ACK�ź�
void IIC_NAck(void);				

//IIC��һ���ֽ�����
unsigned char VL_IIC_Read_1Byte(unsigned char SlaveAddress, unsigned char REG_Address,unsigned char *REG_data);

//IICдn�ֽ�����
unsigned char VL_IIC_Write_nByte(unsigned char SlaveAddress, unsigned char REG_Address,unsigned short len, unsigned char *buf);

//IIC��n�ֽ�����
unsigned char VL_IIC_Read_nByte(unsigned char SlaveAddress, unsigned char REG_Address,unsigned short len,unsigned char *buf);

//VL53l0x д�������
unsigned char VL53l0x_write_multi(unsigned char address, unsigned char index,unsigned char *pdata,unsigned short count);

//VL53l0x ���������
unsigned char VL53l0x_read_multi(unsigned char address,unsigned char index,unsigned char *pdata,unsigned short count);

//VL53l0x д1������(���ֽ�)
unsigned char VL53l0x_write_byte(unsigned char address,unsigned char index,unsigned char data);

//VL53l0x д1������(˫�ֽ�)
unsigned char VL53l0x_write_word(unsigned char address,unsigned char index,unsigned short data);

//VL53l0x д1������(���ֽ�)
unsigned char VL53l0x_write_dword(unsigned char address,unsigned char index,unsigned int data);

//VL53l0x ��1������(���ֽ�)
unsigned char VL53l0x_read_byte(unsigned char address,unsigned char index,unsigned char *pdata);

//VL53l0x ��������(˫�ֽ�)
unsigned char VL53l0x_read_word(unsigned char address,unsigned char index,unsigned short *pdata);

//VL53l0x ��1������(���ֽ�)
unsigned char VL53l0x_read_dword(unsigned char address,unsigned char index,unsigned int *pdata);	

#else

//��ʼ��IIC��IO��
extern void IIC_Init(void);		

//����IIC��ʼ�ź�
extern void IIC_Start(void);				

//����IICֹͣ�ź�
extern void IIC_Stop(void);		

//IIC��һ���ֽ�����
extern unsigned char VL_IIC_Read_1Byte(unsigned char SlaveAddress, unsigned char REG_Address,unsigned char *REG_data);

//IICдn�ֽ�����
extern unsigned char VL_IIC_Write_nByte(unsigned char SlaveAddress, unsigned char REG_Address,unsigned short len, unsigned char *buf);

//IIC��n�ֽ�����
extern unsigned char VL_IIC_Read_nByte(unsigned char SlaveAddress, unsigned char REG_Address,unsigned short len,unsigned char *buf);

//VL53l0x д�������
extern unsigned char VL53l0x_write_multi(unsigned char address, unsigned char index,unsigned char *pdata,unsigned short count);

//VL53l0x ���������
extern unsigned char VL53l0x_read_multi(unsigned char address,unsigned char index,unsigned char *pdata,unsigned short count);

//VL53l0x д1������(���ֽ�)
extern unsigned char VL53l0x_write_byte(unsigned char address,unsigned char index,unsigned char data);

//VL53l0x д1������(˫�ֽ�)
extern unsigned char VL53l0x_write_word(unsigned char address,unsigned char index,unsigned short data);

//VL53l0x д1������(���ֽ�)
extern unsigned char VL53l0x_write_dword(unsigned char address,unsigned char index,unsigned int data);

//VL53l0x ��1������(���ֽ�)
extern unsigned char VL53l0x_read_byte(unsigned char address,unsigned char index,unsigned char *pdata);

//VL53l0x ��������(˫�ֽ�)
extern unsigned char VL53l0x_read_word(unsigned char address,unsigned char index,unsigned short *pdata);

//VL53l0x ��1������(���ֽ�)
extern unsigned char VL53l0x_read_dword(unsigned char address,unsigned char index,unsigned int *pdata);



//VL53l0x д�������
unsigned char VL53l0x_write_multi_t(unsigned char address,unsigned char *pdata,unsigned short count);

//VL53l0x ���������
unsigned char VL53l0x_read_multi_t(unsigned char address,unsigned char *pdata,unsigned short count);

void shut_down_vl53l0x(void);

#endif
















