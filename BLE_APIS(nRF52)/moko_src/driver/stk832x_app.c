#include "stk832x_app.h"
#include "stk832x_driver.h"
#include "spi_io.h"
#include "app_timer.h"
#include "in_flash_manage.h"
#include "iic_io.h"

#include "utility.h"
#include "driver_init.h"


#define MAX_AXES_DATA   10
STK_AxesRaw_t stk_axes[MAX_AXES_DATA];


/*****************************************
*STK832x_Init
********************************************/
void STK832x_Init(void)
{
	nrf_gpio_cfg_output(SPI_CS);
	nrf_gpio_pin_set(SPI_CS);
	software_spi_init(SPI_MISO,SPI_MOSI,SPI_SCLK); 

	
	
	delay_ms(50);
	STK832x_Initialization();
}


/*****************************************
*STK832x_read_data
********************************************/
void STK832x_read_data(void)
{
	uint8_t times = 0;
	uint8_t buf[6],buf_len;
	
	static uint8_t time_cnt=0;
	
	
	
	return;
	
	if(get_is_have_stk832x_sta()==false)  return;
	
	
	if(++time_cnt<2)  return;
	     time_cnt=0;
	
	times = STK832x_Read_fifo_len();	
	STK832x_Getfifo_buf(times,stk_axes);
	
	for(uint8_t i=0;i<times;i++)
	{
		buf_len = 0;
		buf[buf_len++] = stk_axes[i].AXIS_X>>8;
		buf[buf_len++] = stk_axes[i].AXIS_X;
		buf[buf_len++] = stk_axes[i].AXIS_Y>>8;
		buf[buf_len++] = stk_axes[i].AXIS_Y;
		buf[buf_len++] = stk_axes[i].AXIS_Z>>8;
		buf[buf_len++] = stk_axes[i].AXIS_Z;	
	}
}

