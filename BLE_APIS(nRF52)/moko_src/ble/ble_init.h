#ifndef Platform_bleInit_h_
#define Platform_bleInit_h_

#include "ble_gap.h"
#include "stdbool.h"

#define APP_BLE_CONN_CFG_TAG           1  /**< A tag identifying the SoftDevice BLE configuration. */






bool is_ble_connect(void);

void ble_Init(void);
uint16_t get_m_conn_handle(void);
void ble_address_change(void);

#endif

