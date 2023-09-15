#ifndef NORDIC_BLE_H______
#define NORDIC_BLE_H______


#include "bsp.h"

#include "ble_conn_params.h"
#include "ble_advertising.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "app_timer.h"
//#include "es_app_config.h"
#include "app_scheduler.h"
#include "nrf_ble_es.h"
#include "nrf_ble_gatt.h"
//#include "es_battery_voltage.h"
//#include "eddystone_service.h"

#define APP_BLE_OBSERVER_PRIO           3


void ble_init(void);
void power_manage(void);



void start_connect_adv(void);

#endif


