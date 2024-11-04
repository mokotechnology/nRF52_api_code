#include "lis3dh_app.h"
#include "lis3dh_driver.h"
#include "spi52.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

#include "gpio_init.h"
#include "bsp.h"


/*****************************************************/
static bool is_have_3dh = false;
bool  get_3dh_sensor_state(void)
{
   return is_have_3dh;
}

/*************************************************
*INT1_INIT
*********************************************/
static void lis3dh_INT1_init(void)
{
        nrf_gpio_cfg_input(LIS3DH_INT1_PIN,NRF_GPIO_PIN_NOPULL);
	enable_pin_irq(LIS3DH_INT1_PIN,true);

	LIS3DH_SetInt1Pin(LIS3DH_I1_INT1_ON_PIN_INT1_ENABLE);
//	LIS3DH_SetFilterDataSel(MEMS_ENABLE);
	LIS3DH_HPFAOI1Enable(MEMS_ENABLE);
	LIS3DH_WriteReg(INT1_CFG,MOVE_MODE);
	LIS3DH_WriteReg(INT1_THS,20);
	LIS3DH_WriteReg(INT1_DURATION,0x01);
	LIS3DH_Int1LatchEnable(MEMS_DISABLE);
}


/*******************************
*function: 三轴函数
*init spi gpio , spi cs 自个初始化，用模拟spi
********************************/
bool lis3dh_init(void)
{
	uint8_t i,who_am_i=0xff;
	nrf_gpio_cfg_output(SPI_CS);
	nrf_gpio_pin_set(SPI_CS);
	EnableSpi(true,0,SPI_MISO_PIN,SPI_MOSI_PIN,SPI_SCK_PIN);
	
	nrf_delay_ms(20);
	for(i=0;i<3;i++)
	{
		LIS3DH_ReadReg(LIS3DH_WHO_AM_I,&who_am_i); // 三轴是否存在
		if(who_am_i==0x33)
		{
			is_have_3dh = true;
			
			break;
		}
		is_have_3dh = false;	
    }
	BLE_RTT("[3DH] init ID: %x\r\n",who_am_i)
	//is no lis3dh
	if(is_have_3dh == false)   return false;
	
	
	lis3dh_INT1_init();
	
	LIS3DH_SetMode(LIS3DH_NORMAL); 
	LIS3DH_SetAxis(LIS3DH_X_ENABLE|LIS3DH_Y_ENABLE|LIS3DH_Z_ENABLE);
	LIS3DH_SetODR(LIS3DH_ODR_10Hz); // ? ??? LIS3DH_ODR_25Hz
	LIS3DH_SetFullScale(LIS3DH_FULLSCALE_2); // ? ???+-2g
	
	LIS3DH_SetBLE(LIS3DH_BLE_MSB); //  LIS3DH_BLE_MSB  LIS3DH_BLE_LSB
	LIS3DH_SetBDU(MEMS_DISABLE); //BDU
	
	LIS3DH_FIFOModeEnable(LIS3DH_FIFO_STREAM_MODE);
//	LIS3DH_SetWaterMark(31);	

	
	return true;
}





/**********************************************************
*@function: read_data_3dh
**********************************/
#define   XXXX    4
void task_read_3dh(void)
{
    static uint8_t time_cnt=0;
	
	//if device don't have lish3dh sensor, will return 
	if(is_have_3dh==0)  return;
	
	if(++time_cnt<2)  return;
	     time_cnt=0;
	
        EnableSpi(true,0,SPI_MISO_PIN,SPI_MOSI_PIN,SPI_SCK_PIN);
	AxesRaw_t xdata;

	LIS3DH_GetAccAxesRaw(&xdata);

	xdata.AXIS_X =  xdata.AXIS_X >> XXXX;//note! the sign of high it not change.
	xdata.AXIS_Y =  xdata.AXIS_Y >> XXXX;
	xdata.AXIS_Z =  xdata.AXIS_Z >> XXXX;
	
	BLE_RTT("[3DH] X=%dmg  Y=%dmg  Z=%dmg\r\n",xdata.AXIS_X,xdata.AXIS_Y,xdata.AXIS_Z);
}

