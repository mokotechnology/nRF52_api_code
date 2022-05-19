#ifndef driver_init_h_
#define driver_init_h_



#define  TEST_BOARD_PIN 2


#include "SEGGER_RTT.h"

#define BLE_RTT_P

#ifdef BLE_RTT_P
#define BLE_RTT(...) 	SEGGER_RTT_printf(0,__VA_ARGS__);
#else
#define BLE_RTT(...)
#endif



#define BSP_RTT_P

#ifdef BSP_RTT_P
#define BSP_RTT(...) 	SEGGER_RTT_printf(0,__VA_ARGS__);
#else
#define BSP_RTT(...)
#endif





void Platform_driver_init(void);
void Power_manager(void);

#endif


