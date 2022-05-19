#ifndef INFLASH_MANAGE_H__
#define INFLASH_MANAGE_H__


#include "nrf52810_bitfields.h"
#include "nrf_soc.h"

#include <stdint.h>
#include <string.h>
#include "stdbool.h"



#include "SEGGER_RTT.h"

//#define FLASH_RTT_P


#ifdef FLASH_RTT_P
#define FLASH_RTT(...)  SEGGER_RTT_printf(0,__VA_ARGS__);
#else
#define FLASH_RTT(...)
#endif







#define FLASH_USE_SYS_EVT
//#define MAX_FLASH_WRITE_CNT  100
//#define MAX_FLASH_ERASE_CNT  100



/*===========================================================================================
flash:192k  4k
start               end             size                left                	item
0k                  96k             96k                 96k                 	softdevice
96k                 136k           	40k                 56k                		application
136k                176k            40k                 16k                		B_application
176k                180k            4k                  12k                 	hisdata
180k                184k            4k                  8k                  	res_sn1
184k                188k            4k                  4k                  	res_sn2
188k                192k            4k                  0k                 		boot
      

//=========================================================================================
*/

/*===========================================================================================
flash:512k  4k
start               end             size                left                	item
0k                  96k             96k                 416k                 	softdevice
96k                176k           	80                 	336k                	application
176k                180k            4k                  332k                 	hisdata
180k                184k            4k                  328k                  	res_sn1
184k                188k            4k                  324k                  	res_sn2
188k                192k            4k                  320k                 	boot
192k                272k            80                  240k                 	B_application
//=========================================================================================
*/

#define FLASH_SIZE  (192*1024)
//#define FLASH_SIZE  (512*1024)


#define SOFTDEVICE (0)
#define SOFTDEVICE_SIZE (96*1024)

#define APP_ADDR (SOFTDEVICE+SOFTDEVICE_SIZE)


#define HIS_ADDR        (RSV1_ADDR-4096)// 2d000   // store user data is ok  
#define HIS_ADDR_SIZE   (4*1024)

#define RSV1_ADDR        (RSV2_ADDR-4096)// 2E000
#define RSV1_ADDR_SIZE   (4*1024)


#define RSV2_ADDR        (BOOT_ADDR-4096)  // 2F000
#define RSV2_ADDR_SIZE   (4*1024)


#define BOOT_ADDR        (192*1024-4096)
#define BOOT_ADDR_SIZE   (4*1024)



#if ( FLASH_SIZE==(192*1024) ) //52810
#define APP_SIZE (40*1024)
#define APPBVK_ADDR (APP_ADDR+APP_SIZE)
#define APPBVK_SIZE APP_SIZE

#else
#define APP_SIZE (80*1024)
#define APPBVK_ADDR (192*1024)
#define APPBVK_SIZE APP_SIZE
#endif


#define PAGE_SIZE  4096UL
#define BOOT_PARA_ADDR (0x10001014)
#define BOOT__PARA_SIZE  (16)
#define BOOT_PARA_CUSTOM_ADDR (0x10001080)


void call_by_sys_flash_manage(uint32_t sys_evt,void * p_context);
bool Flash_Write_World(uint32_t *addr,uint32_t *Data_Buf,uint32_t len);
uint32_t Flash_ReadData(uint32_t *addr);
void InflashRead(uint8_t *buf,uint32_t addr,uint16_t len);
bool Flash_Erase_Page(uint32_t Page_Num);


void boot_to_new_appilacation(uint8_t* datain,uint8_t len);
void start_dfu(void);

#endif



