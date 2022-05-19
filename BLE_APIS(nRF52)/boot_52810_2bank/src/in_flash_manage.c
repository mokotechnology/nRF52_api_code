#include "in_flash_manage.h"
#include "nrf_nvic.h"
#include "string.h"


//#include "delay.h"
//#include "SoftwareRTC.h"
//#include "nrf52.h"


#define DELAY_MS_FLASH   6
STU_HIS StuHis;


static bool write_success=0;

void call_by_sys_flash_manage(uint32_t sys_evt,void * p_context)
{
    if(NRF_EVT_FLASH_OPERATION_SUCCESS==sys_evt)
        write_success=true;
}

void set_before_operation(void)
{
    write_success=false;
}

//NRF_SDH_SOC_OBSERVER(m_sys_ob88s, BLE_DFU_SOC_OBSERVER_PRIO, call_by_sys_flash_manage,0);




bool  wait_after_operation(void)
{
    uint32_t i;
    //if(is_softdevice_enable()==false) return true;
    for(i=0; i<10000; i++)
    {
        nrf_delay_us(100);
        if(write_success==true)
            return true;
    }



    FLASH_RTT("flash operation timeout ---\r\n");
    return false;
}




// void flash_nvic_page_erase(uint32_t page_address)
//{
//  // Turn on flash erase enable and wait until the NVMC is ready:
//  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);
//  while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
//  {
//      // Do nothing.
//  }
//  // Erase page:
//  NRF_NVMC->ERASEPAGE = page_address;
//  while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
//  {
//      // Do nothing.
//  }
//  // Turn off flash erase enable and wait until the NVMC is ready:
//  NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
//  while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
//  {
//      // Do nothing.
//  }
//}






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




//bool u32equel(uint32_t *addr,uint32_t *Data_Buf,uint32_t len)
//{

//    uint32_t i;
//    for(i=0; i<len; i++)
//    {
//        if(addr[i]!=Data_Buf[i])
//            return false;
//    }
//    return true;
//}


bool CheckAddr(uint32_t addr)
{
    if( (addr>=HIS_ADDR)&&(addr<=BOOT_ADDR))
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
//bool Flash_Write_World(uint32_t *addr,uint32_t *Data_Buf,uint32_t len)
//{
//    uint8_t flash_write_flag;
//    flash_write_flag = flash_write_flag;
//    if(true==CheckAddr((uint32_t)addr))
//    {

//        set_before_operation();
//        flash_write_flag=sd_flash_write(addr,Data_Buf,len);
//        if(0!=flash_write_flag)
//        {
//            FLASH_RTT("write:%x\r\n",flash_write_flag);
//        }
//        return wait_after_operation();

//    }
//    return false;
//}


//uint32_t Flash_ReadData(uint32_t *addr)
//{
//    return *addr;
//}
//void InflashRead(uint8_t *buf,uint32_t addr,uint16_t len)
//{
//    uint16_t i;
//    const uint8_t *pdata;
//    pdata=(uint8_t*)addr;
//    for(i=0; i<len; i++)
//        buf[i]=pdata[i];

//}
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
        set_before_operation();
        flash_erase_flag=sd_flash_page_erase(Page_Num);
        if(0!=flash_erase_flag)
        {
//      FLASH_RTT("erase:%x\r\n",flash_erase_flag);
        }
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
	if(12==len)
	{
		EraseBoot();
		memcpy((uint8_t*)&buf[1],datain,12);
		flash_nvic_word_write((uint32_t*)BOOT_PARA_ADDR,buf[0]);
		flash_nvic_word_write((uint32_t*)(BOOT_PARA_CUSTOM_ADDR+0),buf[1]);
		flash_nvic_word_write((uint32_t*)(BOOT_PARA_CUSTOM_ADDR+4),buf[2]);
		flash_nvic_word_write((uint32_t*)(BOOT_PARA_CUSTOM_ADDR+8),buf[3]);
	}else if(value!=BOOT_ADDR)
	{
		EraseBoot();
		flash_nvic_word_write((uint32_t*)BOOT_PARA_ADDR,buf[0]);
	}


}


//const uint8_t version[3] __attribute__ ((at(APP_ADDR+1024)))= {'c','j','y'};

//void start_dfu(void)
//{
//	uint8_t IapFlg[12];
//	IapFlg[0]=0xaa;
//	IapFlg[1]=0x55;
//	memcpy(IapFlg+2,"caicaihaha",10);
//	boot_to_new_appilacation(IapFlg,12);
//}
//========================================================================================
//#define HIS_BUF_LEN ((sizeof(STU_HIS)+5+3)/4)
//void ResetHis(void)
//{
//    uint8_t *pdes;
//    uint16_t i;
//    pdes=(uint8_t*)&StuHis;

//    for(i=0; i<sizeof(STU_HIS); i++)
//    {
//        pdes[i]=0;
//    }
//    StuHis.adv_interval=10;
//		StuHis.connect_able = 1;
////  StuHis.adv_len=0;

////  StuHis.m_active_slot=0;
////  StuHis.m_lock_state=NRF_BLE_ESCS_LOCK_STATE_LOCKED;
//    AnsyHis();
//}


//format aa aa  len  len data data ......xor
//        0   1  2    3   4   5          len+4
//void InitHis(void)
//{
//    uint32_t buf[HIS_BUF_LEN];
//    uint8_t *pdata;
//    uint8_t *pdata_s;
//    uint8_t *pdata_d;
//    uint8_t data_xor,calc_xor;
//    uint16_t len,len_real,i;
//    InflashRead((uint8_t*)buf,HIS_ADDR,HIS_BUF_LEN*4);

//    pdata=(uint8_t*)buf;


//    if((pdata[0]==HIS_HEAD) &&(pdata[1]==HIS_HEAD))
//    {
//        len_real=sizeof(STU_HIS);
////LOG_DEBUG_PRINTF("len_real=%d,buf_sizeof=%d\r\n",len_real,sizeof(buf));

//        len=pdata[2];
//        len=(len<<8)|pdata[3];

//        if(len_real!=len)
//        {
//            goto HIS_END_LAB;
//        }

//        data_xor=*(pdata+len+4);
//        pdata_s=pdata+4;


//        calc_xor=0;
//        for(i=0; i<len; i++)
//        {
//            calc_xor^=pdata_s[i];
//        }


//        if(calc_xor!=data_xor)
//        {
//            goto HIS_END_LAB;
//        }


//        pdata_d=(uint8_t*)&StuHis;
//        for(i=0; i<len; i++)
//        {
//            pdata_d[i]=pdata_s[i];
//        }
//        return ;
//    }

//HIS_END_LAB:
//    ResetHis();
//    AnsyHis();


//}



//void AnsyHis(void)
//{

//    uint32_t buf[HIS_BUF_LEN];
//    uint8_t *pdata;
//    uint8_t *pdata_src;
//    uint16_t len_real,i,total_len;
//    uint8_t dataxor;
////    uint8_t dataxor_b;
//    pdata=(uint8_t*)buf;

//    pdata[0]=HIS_HEAD;
//    pdata[1]=HIS_HEAD;

//    len_real=sizeof(STU_HIS);
////    dataxor_b=*((uint8_t*)(HIS_ADDR+len_real+4));
//    pdata[2]=len_real>>8;
//    pdata[3]=len_real;




//    pdata_src=(uint8_t*)&StuHis;

//    dataxor=0;
//    for(i=0; i<len_real; i++)
//    {
//        pdata[4+i]=pdata_src[i];
//        dataxor^=pdata_src[i];
//    }
//    pdata[4+len_real]=dataxor;


//    total_len=len_real+5;

//    //if(dataxor_b!=dataxor)
//    {


//        Flash_Erase_Page(HIS_ADDR/PAGE_SIZE);
//        Flash_Write_World((uint32_t *)HIS_ADDR,buf,(total_len+3)/4);
//    }
//}







