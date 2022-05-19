#ifndef NRF_DFU_XX_H___
#define NRF_DFU_XX_H___


#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "ble_dfu.h"
#include "app_error.h"
#include "in_flash_manage.h"
//#include "upfile.h"
//#include "ui_app.h"
//#include "wdt.h"
#include "SEGGER_RTT.h"



//#define DFU_PRINTF
#ifdef DFU_PRINTF
#define DFU_RTT(...) 	SEGGER_RTT_printf(0,__VA_ARGS__);
#else
#define DFU_RTT(...)
#endif


//#define DFU_USE_EXT_FLASH


void dfu_prepare_space(void);//擦除需要更新的位置

void call_by_sys(uint32_t sys_evt,void * p_context);
void dfu_write_flash(uint32_t addr,uint32_t *buf,uint32_t size);//




#include "nrf_dfu.h"
void task_dfu_upfile(void);//放主循环
uint32_t dfu_transport_init(void);
void dfu_transport_callby_irq(ble_evt_t * p_ble_evt);



//使用说明
/*
1:#define DFU_USE_EXT_FLASH 703需要打开这个宏定义，705关闭这个宏
2:task_dfu_upfile放到主循环
3：dfu_transport_init放到协议栈  服务初始化并列一切
4:dfu_transport_callby_irq放到协议栈BLE 回调函数中
5:call_by_sys 如果使用内部flash(既705),这个函数需要放大协议栈事件回调中
*/


#endif


