#ifndef wdt_h_
#define wdt_h_



#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nrf_soc.h"




#define ENLABLE_DOG




void watchdog_init(uint32_t TimeOutMs);
void Feed_WacthDog(void);
#endif

