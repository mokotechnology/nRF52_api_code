#ifndef sht3x_driver_h_
#define sht3x_driver_h_

#include <stdbool.h>

#include <stdint.h>
#include <string.h>




#define   SHT30_SALVE_ADDR      0x88 

#define  SHT30_SDA_PIN    16
#define  SHT30_SCL_PIN    15

#define SHT30_PWR_PIN 5

#define SHT30_PWR_ON()          nrf_gpio_pin_write(SHT30_PWR_PIN,1)
#define SHT30_PWR_OFF()         nrf_gpio_pin_write(SHT30_PWR_PIN,0)

void  sht3x_Init(void);
void  task_read_sht3x(void);
#endif

