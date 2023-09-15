#ifndef  NRF52810_BSP_H__
#define  NRF52810_BSP_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "app_button.h"
#include "app_timer.h"
#include "nrf_delay.h"
#include "nrf_strerror.h"

#include "SEGGER_RTT.h"


#define soft_version  10
#define DEVICE_NAME "BeaconX"

#define BLE_RTT_P
#ifdef BLE_RTT_P
#define BLE_RTT(...) 	SEGGER_RTT_printf(0,__VA_ARGS__);//fprintf(stdout, ">>>>>" format "<<<<", ##__VA_ARGS__)  
#else
#define BLE_RTT(...)
#endif


#define LED_PIN 3
#define LED_ON() nrf_gpio_pin_write(LED_PIN,1)
#define LED_OFF() nrf_gpio_pin_write(LED_PIN,0)
#define LED_TOG() nrf_gpio_pin_toggle(LED_PIN)

#define KEY_PIN 26






//void gpio_init(void);



void delay_ms(uint32_t ms);
void delay_us(uint32_t us);


//LOG--xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

//#include "SEGGER_RTT.h"
//#define DEBUG_DEBUG_PRINTF

#ifdef DEBUG_DEBUG_PRINTF
#define LOG_DEBUG_PRINTF(...) 	SEGGER_RTT_printf(0,__VA_ARGS__);//fprintf(stdout, ">>>>>" format "<<<<", ##__VA_ARGS__)  
#else
#define LOG_DEBUG_PRINTF(...)
#endif
#endif





