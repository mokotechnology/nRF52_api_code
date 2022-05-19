#ifndef gpio_init_h
#define gpio_init_h


#include "nrf_gpio.h"





#define  KEY_PIN            26

#define LED_PIN  3
#define LED_ON()     nrf_gpio_pin_write(LED_PIN,1)
#define LED_OFF()    nrf_gpio_pin_write(LED_PIN,0)

#define LED_TOG()    nrf_gpio_pin_toggle(LED_PIN)






uint8_t get_rx_irq_sta(void);
void clean_rx_irq(void);

void enable_pin_irq(uint32_t pin,bool enable_flg);
void user_gpio_init(void);
#endif
