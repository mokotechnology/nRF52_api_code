#ifndef SPI_IO_H__
#define SPI_IO_H__



#include "stdint.h"




   


#if 0

#define SPI_MOSI    17
#define SPI_MISO    19
#define SPI_SCLK    16
#define SPI_CS      18
#define  LIS3DH_INT1_PIN  13


static uint8_t software_spi_cs = 18;

#else

#define SPI_MOSI    30
#define SPI_MISO    31
#define SPI_SCLK    7
#define SPI_CS      29

#define  LIS3DH_INT1_PIN	28

static uint8_t software_spi_cs = SPI_CS;
#endif


#define    SPI_Enable_CS()        NRF_GPIO->OUTCLR = (1UL << software_spi_cs)  
#define    SPI_Disable_CS()       NRF_GPIO->OUTSET = (1UL << software_spi_cs) 




void software_spi_init(uint8_t mi,uint8_t mo,uint8_t sck);
void software_spi00_send_dat(unsigned char *dat,unsigned int len);
unsigned char software_spi00_receiver_dat(void);
void  software_spi00_receiver_buf(uint8_t *buf,uint32_t len);

void spi_tx_rx(uint8_t *master_data,uint8_t *savle_data,uint8_t len);

void simu_spi_close(void);
#endif


