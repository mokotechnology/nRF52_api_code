#ifndef ble_comm_h_
#define ble_comm_h_

#include "ble_gap.h"




extern ble_gap_addr_t   m_device_addr;





void set_ble_name(void);



void ble_mac_init(void);
void set_ble_mac(uint8_t* p_data);
void get_device_mac(void);



void ble_disconnect(void);
void get_nrf52_chip_temp(void);
uint8_t get_device_type(void);

void task_ble_tx_power(void);
#endif
