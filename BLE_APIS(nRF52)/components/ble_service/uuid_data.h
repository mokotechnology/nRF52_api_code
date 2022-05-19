#ifndef uuid_data_h_
#define uuid_data_h_


#include "ble_nus.h"







void Set_UUID(ble_nus_t * p_nus,ble_nus_evt_t *evt);
void set_major(ble_nus_t * p_nus,ble_nus_evt_t *evt);

void updata_HandleValu(ble_nus_t * p_nus);

void send_to_App(ble_nus_t * p_nus,uint8_t cmd,uint8_t len,uint8_t *data);


void task_device_Factory(void);

#endif

