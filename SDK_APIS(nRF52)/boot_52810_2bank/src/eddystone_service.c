#include "eddystone_service.h"
#include "in_flash_manage.h"
static nrf_ble_escs_t                 m_ble_ecs1;                     





void handle_read(nrf_ble_escs_t * p_escs, uint16_t uuid, uint16_t val_handle);
void handle_write(nrf_ble_escs_t * p_escs, uint16_t uuid,uint16_t val_handle,uint8_t const * p_data,uint16_t length);


void eddystone_service_on_ble(ble_evt_t const * p_ble_evt)
{
 nrf_ble_escs_on_ble_evt(&m_ble_ecs1, p_ble_evt);
}


void eddystone_service_init(void)
{
    uint32_t                 err_code;
    nrf_ble_escs_init_t        ecs_init;
    nrf_ble_escs_init_params_t init_params;
    int8_t                     tx_powers[ESCS_NUM_OF_SUPPORTED_TX_POWER] = ESCS_SUPPORTED_TX_POWER;

    /*Init the broadcast capabilities characteristic*/
    memset(&init_params.broadcast_cap, 0, sizeof(init_params.broadcast_cap));
    init_params.broadcast_cap.vers_byte            = ES_VERSION_BYTE;
    init_params.broadcast_cap.max_supp_total_slots = MAX_SLOTS;
    init_params.broadcast_cap.max_supp_eid_slots   = MAX_EID_SLOTS;
    init_params.broadcast_cap.cap_bitfield         = ( (APP_IS_VARIABLE_ADV_SUPPORTED << ESCS_BROADCAST_VAR_ADV_SUPPORTED_Pos)
                                                   | (APP_IS_VARIABLE_TX_POWER_SUPPORTED << ESCS_BROADCAST_VAR_TX_POWER_SUPPORTED_Pos))
                                                   & (ESCS_BROADCAST_VAR_RFU_MASK);
    init_params.broadcast_cap.supp_frame_types     = ( (APP_IS_URL_SUPPORTED << ESCS_FRAME_TYPE_URL_SUPPORTED_Pos)
                                                   | (APP_IS_UID_SUPPORTED << ESCS_FRAME_TYPE_UID_SUPPORTED_Pos)
                                                   | (APP_IS_TLM_SUPPORTED << ESCS_FRAME_TYPE_TLM_SUPPORTED_Pos)
                                                   | (APP_IS_EID_SUPPORTED << ESCS_FRAME_TYPE_EID_SUPPORTED_Pos))
                                                   & (ESCS_FRAME_TYPE_RFU_MASK);
    memcpy(init_params.broadcast_cap.supp_radio_tx_power, tx_powers, ESCS_NUM_OF_SUPPORTED_TX_POWER);
	
	
    init_params.adv_interval    = 1000;
    init_params.adv_tx_pwr      = 0;
    init_params.radio_tx_pwr    = 0x00;
    init_params.factory_reset   = 0;
    init_params.remain_connectable.r_is_non_connectable_supported = ESCS_FUNCT_REMAIN_CONNECTABLE_SUPPORTED_Yes;

    // Initialize evt handlers and the service
    memset(&ecs_init, 0, sizeof(ecs_init));
    ecs_init.write_evt_handler = handle_write;
    ecs_init.read_evt_handler  = handle_read;
    ecs_init.p_init_vals       = &(init_params);


    m_ble_ecs1.p_active_slot = &StuHis.m_active_slot;
    m_ble_ecs1.p_lock_state  = &StuHis.m_lock_state;

    err_code = nrf_ble_escs_init(&m_ble_ecs1, &ecs_init);
    APP_ERROR_CHECK(err_code);
}


void handle_write(nrf_ble_escs_t     * p_escs,
                                uint16_t        uuid,
                                uint16_t        val_handle,
                                uint8_t const * p_data,
                                uint16_t        length)
{
//    ret_code_t err_code;

    if (p_escs->p_lock_state!=NRF_BLE_ESCS_LOCK_STATE_LOCKED)
    {
//        if (uuid == BLE_UUID_ESCS_UNLOCK_CHAR)
//        {
//            err_code = es_gatts_send_op_not_permitted(p_escs, false);
//            APP_ERROR_CHECK(err_code);
//        }

//        else
//        {
//            err_code = es_gatts_write_handle_unlocked_write(
//                p_escs, uuid, val_handle, p_data, length, m_active_slot);
//            APP_ERROR_CHECK(err_code);
//        }
    }

    else
    {
//        if (uuid == BLE_UUID_ESCS_UNLOCK_CHAR)
//        {
//            err_code = es_gatts_write_handle_unlock(p_escs, p_data, length, val_handle);
//            APP_ERROR_CHECK(err_code);
//        }

//        else
//        {
//            err_code = es_gatts_send_op_not_permitted(p_escs, false);
//            APP_ERROR_CHECK(err_code);
//        }
    }
}


void handle_read(nrf_ble_escs_t * p_escs, uint16_t uuid, uint16_t val_handle)
{
//    ret_code_t err_code;

     if (p_escs->p_lock_state!=NRF_BLE_ESCS_LOCK_STATE_LOCKED)
    {
//        if (uuid == BLE_UUID_ESCS_UNLOCK_CHAR)
//        {
//            err_code = es_gatts_send_op_not_permitted(p_escs, true);
//            APP_ERROR_CHECK(err_code);
//        }

//        else
//        {
//            err_code = es_gatts_read_handle_unlocked_read(p_escs, uuid, val_handle, m_active_slot, m_lock_state);
//            APP_ERROR_CHECK(err_code);
//        }
    }

    else // Beacon is locked.
    {
//        if (uuid == BLE_UUID_ESCS_UNLOCK_CHAR)
//        {
//            err_code = es_gatts_read_handle_unlock(p_escs);
//            APP_ERROR_CHECK(err_code);
//        }

//        else
//        {
//            err_code = es_gatts_read_handle_locked_read(p_escs, uuid, m_lock_state);
//            APP_ERROR_CHECK(err_code);
//        }
    }
}



