#ifndef BSP_H_
#define BSP_H_

#include "SEGGER_RTT.h"


#define DEBUG

#ifdef DEBUG
#define BLE_RTT_P
#define BSP_RTT_P
#endif




#ifdef BLE_RTT_P
#define BLE_RTT(...) 	SEGGER_RTT_printf(0,__VA_ARGS__);
#else
#define BLE_RTT(...)
#endif





#ifdef BSP_RTT_P
#define BSP_RTT(...) 	SEGGER_RTT_printf(0,__VA_ARGS__);
#else
#define BSP_RTT(...)

#endif

#endif