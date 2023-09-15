#include "bsp.h"
#include "nordic_ble.h"
#include "in_flash_manage.h"
#include "boot.h"



#include <stdlib.h>
//#include "nrf52.h"
//#include "nrf52_bitfields.h"
#include "nrf_soc.h"
#include "app_error.h"
#include "app_util.h"
#define ENLABLE_DOG


#ifdef ENLABLE_DOG

static bool B_ReadWdtRunStatues(void)
{
    if(NRF_WDT->RUNSTATUS)
        return true;
    else
        return false;
}
#endif



void V_FeedWdog(void)
{
#ifdef ENLABLE_DOG
    NRF_WDT->RREN=1;
    NRF_WDT->RR[0]=0x6E524635;
    NRF_WDT->RREN=0;
#endif
}
void V_InitWdt(uint32_t TimeOutMs)
{
#ifdef ENLABLE_DOG
    NRF_WDT->TASKS_START=false;
    NRF_WDT->CRV=32768*TimeOutMs/1000-1;
    NRF_WDT->CONFIG=0;
    NRF_WDT->CONFIG|=0x01;//Keep the watchdog running while the CPU is sleeping.
    NRF_WDT->CONFIG|=0x08;//Keep the watchdog running while the CPU is halted by the debugger.
    //NRF_WDT->CONFIG&=0xf7;//Pause watchdog while the CPU is halted by the debugger.
    //NRF_WDT->INTENSET=WDT_INTENSET_TIMEOUT_Set;//???????
    NRF_WDT->TASKS_START=true;
    while(false==B_ReadWdtRunStatues);
    V_FeedWdog();
#endif
}





uint32_t boot_app_addr;
uint32_t boot_app_bvk_addr;
uint32_t size;
uint16_t crc;
uint16_t version_boot;
/*
        IapFlg[0]=0xaa;
        IapFlg[1]=APP_ADDR>>16;
        IapFlg[2]=(uint8_t)(APP_ADDR>>8);
        IapFlg[3]=(uint8_t)APP_ADDR;
        IapFlg[4]=app_size>>16;
        IapFlg[5]=app_size>>8;
        IapFlg[6]=app_size;
        IapFlg[7]=crcout>>8;
        IapFlg[8]=crcout>>0;
		IapFlg[9]=(uint8_t)(APPBVK_ADDR>>16);
        IapFlg[10]=(uint8_t)(APPBVK_ADDR>>8);
        IapFlg[11]=(uint8_t)APPBVK_ADDR;
*/
bool IsNeedUpdata(void)
{
    uint8_t* IapFlg;
    IapFlg=(uint8_t*)(BOOT_PARA_CUSTOM_ADDR);
	if(IapFlg[0]!=0xaa)
		return false;
	
	boot_app_addr = (((uint32_t)IapFlg[1]<<16) | ((uint32_t)IapFlg[2]<<8) |  IapFlg[3]  );
	size = (((uint32_t)IapFlg[4]<<16) | ((uint32_t)IapFlg[5]<<8) |  IapFlg[6]  );
	crc = (((uint32_t)IapFlg[7]<<8) |  IapFlg[8]  );
	boot_app_bvk_addr = (((uint32_t)IapFlg[9]<<16) | ((uint32_t)IapFlg[10]<<8) |  IapFlg[11]  );
	

    return true;
}

void WriteToFlash(void)
{

    uint32_t readtimes,i,pageaddr;
    uint32_t *pdatasrc;
    pdatasrc=(uint32_t *)boot_app_bvk_addr;
    pageaddr=boot_app_addr;
    readtimes=size/PAGE_SIZE;

    if(size%PAGE_SIZE)
        readtimes++;

    for(i=0; i<readtimes; i++)
        flash_nvic_page_erase(pageaddr+i*PAGE_SIZE);

    readtimes=size/4;
    if(size%4)
        readtimes++;
    for(i=0; i<readtimes; i++)
        flash_nvic_word_write((uint32_t *)(pageaddr+4*i),pdatasrc[i]);

}
//uint16_t CalcCrc16(uint16_t crc,const uint8_t* pchMsg, uint32_t wDataLen)
//{
//    uint32_t i;
//    uint8_t j;
//    uint16_t c;
//    for (i=0; i<wDataLen; i++)
//    {
//        c = *(pchMsg+i) & 0x00FF;
//        crc^=c;
//        for (j=0; j<8; j++)
//        {
//            if (crc & 0x0001)
//            {
//                crc >>= 1;
//                crc ^= 0xA001;
//            }
//            else
//            {
//                crc >>= 1;
//            }
//        }
//    }
//    crc = (crc>>8) + (crc<<8);
//    return(crc);
//}
uint16_t crc16_compute_dfu(uint8_t const * p_data, uint32_t size, uint16_t const * p_crc)
{
    uint16_t crc = (p_crc == NULL) ? 0xFFFF : *p_crc;

    for (uint32_t i = 0; i < size; i++)
    {
        crc  = (uint8_t)(crc >> 8) | (crc << 8);
        crc ^= p_data[i];
        crc ^= (uint8_t)(crc & 0xFF) >> 4;
        crc ^= (crc << 8) << 4;
        crc ^= ((crc & 0xFF) << 4) << 1;
    }

    return crc;
}
/*ok return 1 else return 0*/
uint8_t checkok(void)
{
    uint16_t outcrc;
    outcrc=crc16_compute_dfu((const uint8_t *)APP_ADDR,size,NULL);
    if(outcrc==crc)
    {
        return 1;
    }
    return 0;
}

void CleanUpflg(void)
{
    uint8_t buf[12];
    memset(buf,0,12);
    boot_to_new_appilacation(buf,12);
}
void delay_us(uint32_t us)
{
    uint32_t i,j;
    for(j=0; j<us; j++)
    {
        for(i=0; i<10; i++)
        {
            __ASM("NOP");
        }
        __ASM("NOP");
        __ASM("NOP");
        __ASM("NOP");
        __ASM("NOP");
        __ASM("NOP");
        __ASM("NOP");
    }
}


void delay_ms(uint32_t ms)
{
    uint32_t j;
    for(j=0; j<ms; j++)
        delay_us(880);
}
extern void nrf_bootloader_app_start(uint32_t start_addr);
int main(void)
{
    uint8_t i=0;
	
    if(IsNeedUpdata() )
	{
		V_InitWdt(10000);
		i = 0;
        do
        {
            delay_ms(100);
			V_FeedWdog();
            WriteToFlash();
            i++;
        } while((checkok()==0)&&(i<3));
        if(i<3)
        {
            CleanUpflg();
        } 		
		delay_ms(1000);
		NVIC_SystemReset();
		

	}
   
    nrf_bootloader_app_start((96*1024));
}



