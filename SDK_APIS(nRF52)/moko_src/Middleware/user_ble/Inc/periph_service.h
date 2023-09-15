#ifndef periph_service_h_
#define periph_service_h_

#include <stdint.h>
#include <stdbool.h>
#include "sdk_config.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"


#ifdef __cplusplus
extern "C" {
#endif



/**< Used vendor specific UUID. */	
#define NUS_BASE_UUID  {{0xFB, 0x34, 0x9B, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}

	
#define OPCODE_LENGTH        1
#define HANDLE_LENGTH        2
#define BLE_NUS_MAX_DATA_LEN  (NRF_SDH_BLE_GATT_MAX_MTU_SIZE - OPCODE_LENGTH - HANDLE_LENGTH)


// uuid 澶у皬	
#define  UUID_NUMBERS     4
#define  BLE_SEND_LEN   247


#define FEX0_CHAR  0
#define FEX1_NOTIFY  1

/**@brief   Nordic UART Service event types. */
typedef enum
{
    BLE_NUS_EVT_RX_DATA,      /**< Data received. */
    BLE_NUS_EVT_TX_RDY,       /**< Service is ready to accept new data to be transmitted. */
    BLE_NUS_EVT_COMM_STARTED, /**< Notification has been enabled. */
    BLE_NUS_EVT_COMM_STOPPED, /**< Notification has been disabled. */
} ble_nus_evt_type_t;

typedef struct ble_feex_s ble_feex_t;


typedef struct
{
    uint8_t const * p_data; /**< A pointer to the buffer with received data. */
    uint16_t        length; /**< Length of received data. */
} ble_nus_evt_rx_data_t;


//服务数据
typedef void (* ble_feex_data_handler_t) (ble_feex_t * p_nus, uint8_t * p_data, uint16_t length);


typedef struct
{
    ble_feex_data_handler_t data_handler; /**< Event handler to be called for handling received data. */
} ble_feex_init_t;


struct ble_feex_s
{
    uint8_t                         uuid_type;          /**< UUID type for Nordic UART Service Base UUID. */
    uint16_t                        service_handle;     /**< Handle of Nordic UART Service (as provided by the SoftDevice). */
	uint16_t                        conn_handle;  
	
    ble_gatts_char_handles_t        FFFX[UUID_NUMBERS];
	bool                            is_notification_enabled[UUID_NUMBERS];
	
    ble_feex_data_handler_t          data_handler;       /**< Event handler to be called for handling received data. */
};




extern ble_feex_t      m_feex_servive;



void user_service_init(void);
void periph_service_on_ble_evt(ble_feex_t * p_nus, ble_evt_t const * p_ble_evt);
uint32_t Periph_service_send(uint8_t uuid_index,uint8_t *p_data,uint16_t lenth);


uint32_t ble_send_history(uint8_t *p_data,uint16_t lenth);

uint8_t get_notify_history_uuid(void);
#ifdef __cplusplus
}
#endif

#endif // BLE_NUS_H__


