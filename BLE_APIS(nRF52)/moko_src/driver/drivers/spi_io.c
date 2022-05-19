#include "spi_io.h"
#include "nrf_gpio.h"





/**************************/// 这个文件优化代码等级为 0，不优化
static uint8_t software_spi_sclk = 0;
static uint8_t software_spi_si = 0;
static uint8_t software_spi_so = 0;



#define SOFTWARE_SPI_SCLK_SET()		NRF_GPIO->OUTSET=(1<<software_spi_sclk)
#define SOFTWARE_SPI_SCLK_CLR()		NRF_GPIO->OUTCLR=(1<<software_spi_sclk)

#define SOFTWARE_SPI_MOSI_SET()		NRF_GPIO->OUTSET=(1<<software_spi_si)
#define SOFTWARE_SPI_MOSI_CLR()		NRF_GPIO->OUTCLR=(1<<software_spi_si)

#define SOFTWARE_SPI_Read()			(NRF_GPIO->IN&(1<<software_spi_so))




#define _CPOL     0
#define _CPHA     0

void software_spi_init(uint8_t mi,uint8_t mo,uint8_t sck)
{
    software_spi_sclk = sck;
    software_spi_si = mo;
    software_spi_so = mi;

    nrf_gpio_cfg_output(software_spi_sclk);
    nrf_gpio_cfg_output(software_spi_si);
    nrf_gpio_cfg_input(software_spi_so, NRF_GPIO_PIN_NOPULL);
	

#if _CPOL==0&&_CPHA==0
    SOFTWARE_SPI_SCLK_CLR();
#endif
}


/***************************************************
*@function:software_spi00_send_dat
*MODE   0  0
*****************************************/
#if _CPOL==0&&_CPHA==0             
void software_spi00_send_dat(unsigned char *dat,unsigned int len)
{
    unsigned int i;
    unsigned char temp;
	
    for (i=0; i<len; i++)
    {
        temp = dat[i];

        SOFTWARE_SPI_SCLK_CLR();
        if(temp&0x80)			SOFTWARE_SPI_MOSI_SET();
        else					SOFTWARE_SPI_MOSI_CLR();
        SOFTWARE_SPI_SCLK_SET();

        SOFTWARE_SPI_SCLK_CLR();
        if(temp&0x40)			SOFTWARE_SPI_MOSI_SET();
        else					SOFTWARE_SPI_MOSI_CLR();
        SOFTWARE_SPI_SCLK_SET();

        SOFTWARE_SPI_SCLK_CLR();
        if(temp&0x20)			SOFTWARE_SPI_MOSI_SET();
        else					SOFTWARE_SPI_MOSI_CLR();
        SOFTWARE_SPI_SCLK_SET();

        SOFTWARE_SPI_SCLK_CLR();
        if(temp&0x10)			SOFTWARE_SPI_MOSI_SET();
        else					SOFTWARE_SPI_MOSI_CLR();
        SOFTWARE_SPI_SCLK_SET();

        SOFTWARE_SPI_SCLK_CLR();
        if(temp&0x08)			SOFTWARE_SPI_MOSI_SET();
        else					SOFTWARE_SPI_MOSI_CLR();
        SOFTWARE_SPI_SCLK_SET();

        SOFTWARE_SPI_SCLK_CLR();
        if(temp&0x04)			SOFTWARE_SPI_MOSI_SET();
        else					SOFTWARE_SPI_MOSI_CLR();
        SOFTWARE_SPI_SCLK_SET();

        SOFTWARE_SPI_SCLK_CLR();
        if(temp&0x02)			SOFTWARE_SPI_MOSI_SET();
        else					SOFTWARE_SPI_MOSI_CLR();
        SOFTWARE_SPI_SCLK_SET();

        SOFTWARE_SPI_SCLK_CLR();
        if(temp&0x01)			SOFTWARE_SPI_MOSI_SET();
        else					SOFTWARE_SPI_MOSI_CLR();
        SOFTWARE_SPI_SCLK_SET();
    }
    SOFTWARE_SPI_SCLK_CLR();//end
}


unsigned char software_spi00_receiver_dat(void)
{
    unsigned char dat=0;

    SOFTWARE_SPI_SCLK_SET();
    if(SOFTWARE_SPI_Read())		dat|=0x80;
    SOFTWARE_SPI_SCLK_CLR();

    SOFTWARE_SPI_SCLK_SET();
    if(SOFTWARE_SPI_Read())		dat|=0x40;
    SOFTWARE_SPI_SCLK_CLR();

    SOFTWARE_SPI_SCLK_SET();
    if(SOFTWARE_SPI_Read())		dat|=0x20;
    SOFTWARE_SPI_SCLK_CLR();

    SOFTWARE_SPI_SCLK_SET();
    if(SOFTWARE_SPI_Read())		dat|=0x10;
    SOFTWARE_SPI_SCLK_CLR();

    SOFTWARE_SPI_SCLK_SET();
    if(SOFTWARE_SPI_Read())		dat|=0x08;
    SOFTWARE_SPI_SCLK_CLR();

    SOFTWARE_SPI_SCLK_SET();
    if(SOFTWARE_SPI_Read())		dat|=0x04;
    SOFTWARE_SPI_SCLK_CLR();

    SOFTWARE_SPI_SCLK_SET();
    if(SOFTWARE_SPI_Read())		dat|=0x02;
    SOFTWARE_SPI_SCLK_CLR();

    SOFTWARE_SPI_SCLK_SET();
    if(SOFTWARE_SPI_Read())		dat|=0x01;
    SOFTWARE_SPI_SCLK_CLR();


    SOFTWARE_SPI_SCLK_CLR();//end
    return dat;
}

void  software_spi00_receiver_buf(uint8_t *buf,uint32_t len)
{
    uint32_t i;
    for (i=0; i<len; i++)
    {
        buf[i] = software_spi00_receiver_dat();
    }

}
#endif


/*模式二           写数据*/
#if _CPOL==1&&_CPHA==0           //MODE   1  0
void SPI_Send_Dat(unsigned char dat)
{
    unsigned char n;
    for(n=0; n<8; n++)
    {
        SCK_D(1);
        if(dat&0x80)MOSI_D(1);
        else MOSI_D(0);
        dat<<=1;
        SCK_D(0);
    }
    SCK_D(1);
}
/*模式二          读数据*/
unsigned char SPI_Receiver_Dat(void)
{
    unsigned char n ,dat,bit_t;
    for(n=0; n<8; n++)
    {
        SCK_D(1);
        dat<<=1;
        if(MISO_I())dat|=0x01;
        else dat&=0xfe;
        SCK_D(0);
    }
    SCK_D(1);
    return dat;
}

#endif
/*模式一        写数据*/
#if _CPOL==0&&_CPHA==1           //MODE  0  1
void SPI_Send_Dat(unsigned char dat)
{
    unsigned char n;
    SCK_D(0);
    for(n=0; n<8; n++)
    {
        SCK_D(1);
        if(dat&0x80)MOSI_D(1);
        else MOSI_D(0);
        dat<<=1;
        SCK_D(0);
    }
}
/*模式一       读数据*/
unsigned char SPI_Receiver_Dat(void)
{
    unsigned char n ,dat,bit_t;
    for(n=0; n<8; n++)
    {
        SCK_D(1);
        dat<<=1;
        if(MISO_I())dat|=0x01;
        else dat&=0xfe;
        SCK_D(0);
    }
    SCK_D(0);
    return dat;
}
#endif


#if _CPOL==1&&_CPHA==1            //MODE  1  1
/*模式三          写数据*/
void SPI_Send_Dat(unsigned char dat)
{
    unsigned char n;
    SCK_D(1);
    for(n=0; n<8; n++)
    {
        SCK_D(0);
        if(dat&0x80)MOSI_D(1);
        else MOSI_D(0);
        dat<<=1;
        SCK_D(1);
    }
}
/*模式三          读数据*/
unsigned char SPI_Receiver_Dat(void)
{
    unsigned char n ,dat,bit_t;
    SCK_D(0);
    for(n=0; n<8; n++)
    {   SCK_D(0);
        dat<<=1;
        if(MISO_I())dat|=0x01;
        else dat&=0xfe;
        SCK_D(1);
    }
    SCK_D(1);
    return dat;
}
#endif




