#ifndef EDDSYSTONE__H________
#define EDDSYSTONE__H________

#include "stdint.h"
#include "stdbool.h"
#include "es.h"
#include "nrf_ble_escs.h"


#define MAX_SLOTS        5
#define MAX_EID_SLOTS  5
#define ES_VERSION_BYTE 1

// Broadcast Capabilities
#define APP_IS_VARIABLE_ADV_SUPPORTED       ESCS_BROADCAST_VAR_ADV_SUPPORTED_No         //!< Information whether variable advertisement intervals are supported.
#define APP_IS_VARIABLE_TX_POWER_SUPPORTED  ESCS_BROADCAST_VAR_TX_POWER_SUPPORTED_Yes   //!< Information whether variable advertisement TX power is supported.

#define APP_IS_UID_SUPPORTED                ESCS_FRAME_TYPE_UID_SUPPORTED_Yes           //!< Information whether the UID frame is supported.
#define APP_IS_URL_SUPPORTED                ESCS_FRAME_TYPE_URL_SUPPORTED_Yes           //!< Information whether the URL frame is supported.
#define APP_IS_TLM_SUPPORTED                ESCS_FRAME_TYPE_TLM_SUPPORTED_Yes           //!< Information whether the TLM frame is supported.
#define APP_IS_EID_SUPPORTED                ESCS_FRAME_TYPE_EID_SUPPORTED_Yes           //!< Information whether the EID frame is supported.



void eddystone_service_init(void);
//void eddystone_service_on_ble(ble_evt_t const * p_ble_evt);
#endif



