#include "battery_check.h"
#include "nrf_drv_saadc.h"
#include "nrf_delay.h"
#include "ble_bas.h"
#include "ble_err.h"




/********************************************/
extern  ble_bas_t   m_bas;  



int avg_sample;
nrf_saadc_value_t m_buffer_pool[10];
uint32_t busy_ms;


static uint16_t    batt_adc_valu  = 0; //!< Current battery.
static uint8_t     batt_level = 0;   // 100% 


/***************************************************************************************************/
#define ADC_REF  600  //!< Reference voltage (in milli volts) used by ADC while doing conversion.
#define ADC_10BIT                  1024 //!< Maximum digital value for 10-bit ADC conversion.
#define ADC_gain  6    //!< The ADC is configured to use VDD with 1/3 prescaling as input. And hence the result of conversion is to be multiplied by 3 to get the actual value of the battery voltage.

#define ADC_valu(ADC_VALUE)                 \
        ((((ADC_VALUE) *ADC_REF) / ADC_10BIT) * ADC_gain)
		
		
/***********************************************************
*@function:saadc_callback
*@description:callback adc value
************************************************************/
void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
		avg_sample = p_event->data.done.p_buffer[0];
		batt_adc_valu = (ADC_valu(avg_sample) + 0);// realy adc src + calibration value.
    }
}

/***********************************************************
*@function:InitAdc
*@description:
************************************************************/
void InitAdc(nrf_saadc_input_t adc_chann)
{
    ret_code_t err_code;

    err_code=nrf_drv_saadc_init(0,saadc_callback);
    APP_ERROR_CHECK(err_code);
	nrf_saadc_channel_config_t channel_cfg=NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(adc_chann);
   
	err_code=nrf_drv_saadc_channel_init(0,&channel_cfg);
    APP_ERROR_CHECK(err_code);
}

/*******************************************
*@function:StartAdc
*@description:start read adc valu
****************************************************/
void StartAdc(nrf_saadc_input_t input,uint8_t times)
{
    static uint8_t first=1; 
	if(first)
	{
		first=0;
		InitAdc(input);
	}
	nrf_drv_saadc_buffer_convert(m_buffer_pool, 1);
	nrf_drv_saadc_sample();
  
    busy_ms=0;
	while(true==nrf_drv_saadc_is_busy())
	{
		busy_ms++;
		nrf_delay_ms(1);
	}
}

/****************************************
*@function    :get_battery_valu
*@description :[data out]return battery adc value
***************************************/
uint16_t get_battery_valu(void)
{
	return batt_adc_valu;
}

/****************************************
*@function    :get_battery_level
*@description :[data out]return battery level
***************************************/
uint16_t get_battery_level(void)
{
	return batt_level;
}

/********************************************
*@function    : Read_battery
*@description : device read battery valu 
*********************************************/
static void read_battery(void)
{
	StartAdc(NRF_SAADC_INPUT_VDD,5);
	
	uint8_t level = 0;
	uint16_t  temp_adc_valu = 0;
	
	temp_adc_valu = get_battery_valu();
	
	if(temp_adc_valu >= 3000)
	{
		level = 100;
	} 
	else if(temp_adc_valu > 2000)
	{
		level = (temp_adc_valu - 2000)/ 10;
	}	  
	else
	{
		level = 0;
	}
	batt_level = level;
}

/****************************************
*@function    : task_Read_battery
*@description : @1: device init(read one time) 
*              @2: after 30s (read one time) 
*              @3: after 30min read.............go on 
******************************************/
void ble_bas_battery_update(void);
void task_read_battery(void)
{
	static uint16_t time_cnt = 0;
	static uint8_t  read_flg = 0; 
	static uint8_t  sta=1;
		
	switch(sta)
	{
		case 1:
            read_battery();
		    sta = 2;
			break;		
		case 2:
			if(++time_cnt>=30)
			{
				read_battery();								
				read_flg = 1;
				time_cnt = 0;
				sta = 3;
			}
			break;			
		case 3:
			if(++time_cnt>=1800)
			{
				read_battery();
				time_cnt = 0;
				read_flg = 1;
				
				sta = 3;
			}
			break;
		default:
			break;
	}	
	//updata battery service handle valu and low battery check 
	if(read_flg)
	{
		read_flg =0;
		ble_bas_battery_update();
	}
}









