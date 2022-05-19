#ifndef INFLASH_MANAGE_H__
#define INFLASH_MANAGE_H__

#include "stdint.h"
#include "nrf52810_bitfields.h"
#include "nrf_soc.h"
#include "bsp.h"


//#include "SEGGER_RTT.h"
//#define FLASH_RTT_P

#ifdef FLASH_RTT_P
#define FLASH_RTT(...) 	SEGGER_RTT_printf(0,__VA_ARGS__);
#else
#define FLASH_RTT(...)
#endif

#define FLASH_USE_SYS_EVT
//#define MAX_FLASH_WRITE_CNT  100
//#define MAX_FLASH_ERASE_CNT  100




/*===========================================================================================
flash:192k  24k
start		    end		    size		left		item
0			    	96k		96k			96k		softdevice
96k			    160k		64k			32k		application
160k  			164k        4k			28k   hisdata
164k  			168k        4k			24k   res_sn保留使用
168k			180k	    16k			16k  	left




180k				192k				12k			0k		boot

//=========================================================================================
*/
//
#define PAGE_SIZE  4096UL

#define APP_ADDR (96*1024)
//#define APP_SIZE (64*1024)

//#define HIS_ADDR  (APP_ADDR+APP_SIZE)
//#define HIS_ADDR_SIZE   (4*1024)

//#define REV_SN_ADDR  (HIS_ADDR+HIS_ADDR_SIZE)
//#define REV_SN_SIZE   (4*1024)


//#define BOOT_ADDR        (192*1024-12*1024)      /* 0X30000  0X4000=0X2c000*/
//#define BOOT_ADDR_SIZE   (12*1024)

#define BOOT_ADDR        (192*1024-4096)
#define BOOT_ADDR_SIZE   (4*1024)



#define BOOT_PARA_ADDR (0x10001014)
#define BOOT__PARA_SIZE  (16)

#define BOOT_PARA_CUSTOM_ADDR (0x10001080)




#pragma pack(1)
typedef struct
{
uint8_t m_active_slot;//不需要实时保存
uint8_t m_lock_state;
uint8_t adv_interval;/*100ms的倍数*/
int8_t tx_power;
uint8_t connect_able;/* 00-->unconnectable 01--->connectable*/
} STU_HIS;

#pragma pack()


extern STU_HIS StuHis;


void call_by_sys_flash_manage(uint32_t sys_evt,void * p_context);
bool Flash_Write_World(uint32_t *addr,uint32_t *Data_Buf,uint32_t len);
uint32_t Flash_ReadData(uint32_t *addr);
void InflashRead(uint8_t *buf,uint32_t addr,uint16_t len);
bool Flash_Erase_Page(uint32_t Page_Num);

#define HIS_HEAD 0xa4
void ResetHis(void);
void InitHis(void);
void AnsyHis(void);


void boot_to_new_appilacation(uint8_t* datain,uint8_t len);
void start_dfu(void);


#endif



