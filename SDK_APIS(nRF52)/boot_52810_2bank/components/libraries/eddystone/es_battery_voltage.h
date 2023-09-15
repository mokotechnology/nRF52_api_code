
#ifndef ES_BATTERY_VOLTAGE_H__
#define ES_BATTERY_VOLTAGE_H__

#include <stdint.h>
#include <stdbool.h>
#include <nrf_delay.h>

void InitAdc(void);

uint16_t GetBattery(void);


#endif // ES_BATTERY_VOLTAGE_H__
