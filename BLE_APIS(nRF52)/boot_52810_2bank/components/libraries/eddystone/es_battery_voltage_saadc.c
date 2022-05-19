
#include "es_battery_voltage.h"
#include "nrf_drv_saadc.h"
#include "bsp.h"

#define ADC_REF_VOLTAGE_IN_MILLIVOLTS  600  //!< Reference voltage (in milli volts) used by ADC while doing conversion.
#define DIODE_FWD_VOLT_DROP_MILLIVOLTS 270  //!< Typical forward voltage drop of the diode (Part no: SD103ATW-7-F) that is connected in series with the voltage supply. This is the voltage drop when the forward current is 1mA. Source: Data sheet of 'SURFACE MOUNT SCHOTTKY BARRIER DIODE ARRAY' available at www.diodes.com.
#define ADC_RES_10BIT                  1024 //!< Maximum digital value for 10-bit ADC conversion.
#define ADC_PRE_SCALING_COMPENSATION   6    //!< The ADC is configured to use VDD with 1/3 prescaling as input. And hence the result of conversion is to be multiplied by 3 to get the actual value of the battery voltage.
#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE) \
    ((((ADC_VALUE) *ADC_REF_VOLTAGE_IN_MILLIVOLTS) / ADC_RES_10BIT) * ADC_PRE_SCALING_COMPENSATION)





static int16_t avg_sample;
void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{

    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        avg_sample= p_event->data.done.p_buffer[0];
        if(avg_sample<0)
        avg_sample=0;
    }
}
void InitAdc(void)
{
    ret_code_t err_code;

    err_code=nrf_drv_saadc_init(0,saadc_callback);
    APP_ERROR_CHECK(err_code);
    nrf_saadc_channel_config_t channel_cfg=NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_VDD);
    nrf_drv_saadc_channel_init(0,&channel_cfg);
}

uint16_t GetBattery(void)
{
    uint8_t i=0;
    int16_t batt;

    nrf_saadc_value_t m_buffer_pool[10];

    nrf_drv_saadc_buffer_convert(m_buffer_pool,1);
    nrf_drv_saadc_sample();
    while(true==nrf_drv_saadc_is_busy())
    {
    nrf_delay_ms(1);
    if(i++>100)
    return 3300;
    }

    batt= ADC_RESULT_IN_MILLI_VOLTS(avg_sample) + DIODE_FWD_VOLT_DROP_MILLIVOLTS;
    BLE_RTT("vol:%d\r\n",batt);
    return batt;

}



