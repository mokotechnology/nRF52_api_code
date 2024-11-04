#include "spi52.h"
#include "nrf_gpio.h"



/************************************************************
*  EnableSpi
*
**********************************************/
void EnableSpi(bool en_fg,uint8_t mode,uint8_t miso_pin,uint8_t mosi_pin,uint8_t sck_pin)
{
    NRF_SPIM_Type * p_spim = NRF_SPIM0;

    if(true==en_fg)
    {
        if (mode <= NRF_SPIM_MODE_1)
        {
            nrf_gpio_pin_clear(sck_pin);
        } else
        {
            nrf_gpio_pin_set(sck_pin);
        }
		
        nrf_gpio_cfg(sck_pin,
                     NRF_GPIO_PIN_DIR_OUTPUT,
                     NRF_GPIO_PIN_INPUT_CONNECT,
                     NRF_GPIO_PIN_NOPULL,
                     NRF_GPIO_PIN_S0S1,
                     NRF_GPIO_PIN_NOSENSE);
        // - MOSI (optional) - output with initial value 0,
        if (mosi_pin != 0xff)
        {
            nrf_gpio_pin_clear(mosi_pin);
            nrf_gpio_cfg_output(mosi_pin);
        }

        // - MISO (optional) - input,
        if (miso_pin != 0xff)
        {
            miso_pin = miso_pin;
            nrf_gpio_cfg_input(miso_pin, NRF_GPIO_PIN_PULLUP);
        }

        //hardware_spim
        nrf_spim_pins_set(p_spim, sck_pin, mosi_pin, miso_pin);
        nrf_spim_frequency_set(p_spim,NRF_SPIM_FREQ_8M);
        nrf_spim_configure(p_spim,(nrf_spim_mode_t)mode, (nrf_spim_bit_order_t)NRF_SPIM_BIT_ORDER_MSB_FIRST);

        nrf_spim_orc_set(p_spim, 0xff);

        nrf_spim_tx_list_enable(p_spim);
        nrf_spim_rx_list_enable(p_spim);


        //nrf_spim_int_enable(p_spim, NRF_SPIM_INT_END_MASK);
        nrf_spim_enable(p_spim);
    }
    else
    {
        nrf_spim_disable(p_spim);
		
//			nrf_gpio_cfg_default(miso_pin);
//		nrf_gpio_cfg_input(miso_pin, NRF_GPIO_PIN_PULLUP);
		
//	nrf_gpio_cfg_default(miso_pin);
//	nrf_gpio_cfg_input(miso_pin, NRF_GPIO_PIN_PULLUP);	
		
//			nrf_gpio_cfg_default(sck_pin);
//	nrf_gpio_cfg_input(sck_pin, NRF_GPIO_PIN_PULLUP);	
		
		
//	nrf_gpio_cfg_default(SPI_CS_FLASH);
//	nrf_gpio_cfg_input(SPI_CS_FLASH, NRF_GPIO_PIN_PULLUP);		
		
    }

}



/*******************************************
*spi_send
*
************************************************************/
bool spi_send(uint8_t spi_num,uint8_t* p_tx_buffer,uint8_t tx_length,uint8_t *p_rx_buffer,uint8_t rx_length)
{
    NRF_SPIM_Type * p_spim = NRF_SPIM0;
    uint32_t delay_us=2000000;
    nrf_spim_tx_buffer_set(p_spim, p_tx_buffer, tx_length);
    nrf_spim_rx_buffer_set(p_spim, p_rx_buffer, rx_length);

    nrf_spim_event_clear(p_spim, NRF_SPIM_EVENT_END);

    nrf_spim_task_trigger(p_spim, NRF_SPIM_TASK_START);

    while (((delay_us--)>2) && (!nrf_spim_event_check(p_spim, NRF_SPIM_EVENT_END)) )
    {

    }
    if (delay_us>10)
        return true;

    return false;
}


/*******************************************
*spi_send_read1
*
************************************************************/

static uint32_t g_delay_us=1000;
void spi_send_read1(uint8_t spi_num,uint8_t* in,uint8_t inlen,uint8_t *out,uint8_t out_len)
{
    //hardware_spi_transmission_no_wait(NRF_SPIM0,in,inlen,out,out_len);
    NRF_SPIM_Type * p_spim = NRF_SPIM0;
    g_delay_us=1000;//2000000;
	
    nrf_spim_tx_buffer_set(p_spim, in, inlen);
    nrf_spim_rx_buffer_set(p_spim, out, out_len);

    nrf_spim_event_clear(p_spim, NRF_SPIM_EVENT_END);

    nrf_spim_task_trigger(p_spim, NRF_SPIM_TASK_START);
}


/*******************************************
*wait_read
*
************************************************************/
bool wait_read(void)
{
    NRF_SPIM_Type * p_spim = NRF_SPIM0;
    while (   ((g_delay_us--)>2) && (!nrf_spim_event_check(p_spim, NRF_SPIM_EVENT_END)) )
    {
		
    }
    if (g_delay_us>10)
	{
        return true;
	}
	else
	{
		//EX_FLASH_RTT("-->g_delay_us=%d\r\n",g_delay_us);
	}
    return false;
}





