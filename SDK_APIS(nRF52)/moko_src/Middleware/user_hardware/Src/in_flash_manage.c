#include "in_flash_manage.h"
#include "nrf_delay.h"

#include "nrf_nvic.h"
//#include "nrf52.h"
#include "string.h"


/****************************************************************/
static bool write_success=0;

/********************************************
*call_by_sys_flash_manage
**************************************/
void call_by_sys_flash_manage(uint32_t sys_evt,void * p_context)
{
    if(NRF_EVT_FLASH_OPERATION_SUCCESS==sys_evt)
        write_success=true;
}

void set_before_operation(void)
{
    write_success=false;
}
bool wait_after_operation(void)
{
    uint32_t i;
    //if(is_softdevice_enable()==false) return true;
    for(i=0; i<10000; i++)
    {
        nrf_delay_us(100);
        if(write_success==true)
		{
			FLASH_RTT("flash operate success\r\n");
            return true;
		}
    }
	FLASH_RTT("flash operate err\r\n");
    return false;
}


bool u32equel(uint32_t *addr,uint32_t *Data_Buf,uint32_t len)
{

    uint32_t i;
    for(i=0; i<len; i++)
    {
        if(addr[i]!=Data_Buf[i])
            return false;
    }
    return true;
}

/********************************************
*@function:
**************************************/
bool CheckAddr(uint32_t addr)
{

#if ( FLASH_SIZE==(192*1024) )
    if( (addr>=APPBVK_ADDR)&&(addr<BOOT_ADDR))
#else
    if( ( (addr>=HIS_ADDR)&&(addr<=BOOT_ADDR))||(addr>=APPBVK_ADDR))
#endif
    {
        return true;

    }
    return false;
}
/*************************************************************************************************
* function name: uint8_t Flash_Write_World(uint32_t *addr,uint32_t *Data_Buf,uint32_t len)
* function describe :  write data to inflash with softdevice
*
* author: liupeng
*
* version: 1.0
* return describe:
**************************************************************************************************/
bool Flash_Write_World(uint32_t *addr,uint32_t *Data_Buf,uint32_t len)
{
    uint8_t flash_write_flag;
    flash_write_flag = flash_write_flag;
	
    if(true==CheckAddr((uint32_t)addr))
    {
		FLASH_RTT("flash write:%x page:%d len:%d\r\n",(uint32_t )addr,((uint32_t )addr)/4096,len);
		
        set_before_operation();
		
        flash_write_flag=sd_flash_write(addr,Data_Buf,len);
		
        if(0!=flash_write_flag)
        {
//            FLASH_RTT("write:%x\r\n",flash_write_flag);
        }
        return wait_after_operation();
    }
    return false;
}


uint32_t Flash_ReadData(uint32_t *addr)
{
    return *addr;
}

void InflashRead(uint8_t *buf,uint32_t addr,uint16_t len)
{
    uint16_t i;
    const uint8_t *pdata;
    pdata=(uint8_t*)addr;
    for(i=0; i<len; i++)
        buf[i]=pdata[i];

}
/*************************************************************************************************
* function name: uint8_t Flash_Erase_Page(uint32_t Page_Num)
* function describe :  erase inflash with softdevice
*
* author: liupeng
*
* version: 1.0
* return describe:
**************************************************************************************************/
bool Flash_Erase_Page(uint32_t Page_Num)
{
    uint32_t addr;
    uint8_t flash_erase_flag;
    addr=Page_Num*PAGE_SIZE;

    flash_erase_flag=flash_erase_flag;
	
    if(true==CheckAddr((uint32_t)addr))
    {
		FLASH_RTT("ERASE PAGE:%d\r\n",Page_Num);
        set_before_operation();
			
        flash_erase_flag=sd_flash_page_erase(Page_Num);
  
        FLASH_RTT("erase page :%d  result:0x%x\r\n",Page_Num,flash_erase_flag);
        nrf_delay_ms(50);
        return wait_after_operation();
    }
    return false;
}

//=====================================================================================================================================
//boot
void EraseBoot(void)
{

    // Turn on flash erase enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
    // Erase page:
    NRF_NVMC->ERASEUICR = 1;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
    // Turn off flash erase enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}


void flash_nvic_word_write(uint32_t *address, uint32_t value)
{
    // Turn on flash write enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
    *address = value;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
    // Turn off flash write enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}



void boot_to_new_appilacation(uint8_t* datain,uint8_t len)
{
    uint32_t buf[4];

    uint32_t value;

    value=*((uint32_t*)BOOT_PARA_ADDR);
    buf[0]=BOOT_ADDR;
	
    if(12 == len)
    {
        EraseBoot();
        memcpy((uint8_t*)&buf[1],datain,12);
		
        flash_nvic_word_write((uint32_t*)BOOT_PARA_ADDR,buf[0]);
        flash_nvic_word_write((uint32_t*)(BOOT_PARA_CUSTOM_ADDR+0),buf[1]);
        flash_nvic_word_write((uint32_t*)(BOOT_PARA_CUSTOM_ADDR+4),buf[2]);
        flash_nvic_word_write((uint32_t*)(BOOT_PARA_CUSTOM_ADDR+8),buf[3]);
    }
    else if(value!=BOOT_ADDR)
    {
        EraseBoot();
        flash_nvic_word_write((uint32_t*)BOOT_PARA_ADDR,buf[0]);
    }
}


//const uint8_t version[3] __attribute__ ((at(APP_ADDR+1024)))= {'c','j','y'};
const uint8_t version[3]  __attribute__((section(".dfu_data_check"))) = {'c','j','y'};

void start_dfu(void)
{
    uint8_t IapFlg[12];
    IapFlg[0]=0xaa;
    IapFlg[1]=0x55;
	
    memcpy(IapFlg+2,"caicaihaha",10);
    boot_to_new_appilacation(IapFlg,12);
}




//========================================================================================
//static uint8_t save_factory_data_flg=1;
//#define FACTORY_SET_HEAD 0xaaaaaaaa
//static void write_factory_datas(uint32_t *pdata)
//{
//    uint32_t data_head=FACTORY_SET_HEAD;
//    if(save_factory_data_flg)
//    {
//		save_factory_data_flg=0;
//		Flash_Erase_Page(RSV1_ADDR/PAGE_SIZE);
//		Flash_Write_World((uint32_t *)RSV1_ADDR,&data_head,1);
////		Flash_Write_World((uint32_t *)(RSV1_ADDR+4),pdata,(sizeof(STU_HIS)+3)/4);
//    }
//}







