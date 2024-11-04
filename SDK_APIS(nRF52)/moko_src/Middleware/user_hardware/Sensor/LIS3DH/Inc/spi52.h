#ifndef SPI52_H__
#define SPI52_H__

#include "nrf_drv_spi.h"
#include "app_error.h"
#include "string.h"
#include "bsp.h"
#include "nrf_gpio.h"

//#define SPI_CS_3DH      						29
//#define ACC_CS_HIGH() 							nrf_gpio_pin_set(SPI_CS_3DH)
//#define ACC_CS_LOW() 							nrf_gpio_pin_clear(SPI_CS_3DH)

#define SPI_MOSI_PIN    						30   
#define SPI_MISO_PIN   							31    
#define SPI_SCK_PIN    							7  

//#define MODE_LCD_SPI 10


bool spi_send(uint8_t spi_num,uint8_t* in,uint8_t inlen,uint8_t *out,uint8_t out_len);
void EnableSpi(bool en_fg,uint8_t mode,uint8_t mi,uint8_t mo,uint8_t sck);


void spi_send_read1(uint8_t spi_num,uint8_t* in,uint8_t inlen,uint8_t *out,uint8_t out_len);
bool wait_read(void);



#endif
