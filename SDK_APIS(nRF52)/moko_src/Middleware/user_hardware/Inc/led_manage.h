#ifndef led_manage_h_
#define led_manage_h_



#include "string.h"
#include "stdint.h"



#define LED_TIMER  0x01


void led_init(void);




void led_timer_init(void);
void led_ctrl(uint16_t on_ms,uint16_t off_ms,uint16_t times);
void led_timer_manage(void);
#endif


