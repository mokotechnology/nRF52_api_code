/**
 * Copyright (c) 2012 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include "sdk_common.h"
#if NRF_MODULE_ENABLED(BLE_NUS)
#include "ble.h"
#include "ble_nus.h"
#include "ble_srv_common.h"
#include "uuid_init.h"
#include "uuid_data.h"
#include "adv_init.h"
#include "key_led.h"
#include "system_time.h"



#define BLE_NUS_MAX_RX_CHAR_LEN        BLE_NUS_MAX_DATA_LEN        /**< Maximum length of the RX Characteristic (in bytes). */
#define BLE_NUS_MAX_TX_CHAR_LEN        BLE_NUS_MAX_DATA_LEN        /**< Maximum length of the TX Characteristic (in bytes). */

#define NUS_BASE_UUID  {0xFB, 0x34, 0x9B, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}/**< Used vendor specific UUID. */









/************************/
/**@brief Function for handling the @ref BLE_GAP_EVT_CONNECTED event from the SoftDevice.
 *
 * @param[in] p_nus     Nordic UART Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_connect(ble_nus_t * p_nus, ble_evt_t const * p_ble_evt)
{
    p_nus->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the @ref BLE_GAP_EVT_DISCONNECTED event from the SoftDevice.
 *
 * @param[in] p_nus     Nordic UART Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_disconnect(ble_nus_t * p_nus, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_nus->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for handling the @ref BLE_GATTS_EVT_WRITE event from the SoftDevice.
 *
 * @param[in] p_nus     Nordic UART Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_write(ble_nus_t * p_nus, ble_evt_t const *p_ble_evt)  // const
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	
    ble_nus_evt_t evt;
    evt.p_nus = p_nus;
	
	evt.params.rx_data.p_data = p_evt_write->data;
    evt.params.rx_data.length = p_evt_write->len;
	
	
    if ((p_evt_write->handle == p_nus->Rx_hand.cccd_handle)
        && (p_evt_write->len == 2))
    {
        if (ble_srv_is_notification_enabled(p_evt_write->data))
        {
            p_nus->is_notification_enabled = true;
        }
        else
        {
            p_nus->is_notification_enabled = false;
        }
    }

/********************************************************************************/	
    else if ((p_evt_write->handle == p_nus->uuid_hand.value_handle) // uuid 
                                     &&(p_nus->data_handler != NULL))
    {
		Set_UUID(p_nus,&evt);
    }

	else if ((p_evt_write->handle == p_nus->major_hand.value_handle) //  
                                    &&(p_nus->data_handler != NULL))
    {
		set_major(p_nus,&evt);
    }
}

/**@brief Function for adding TX characteristic.
 *
 * @param[in] p_nus       Nordic UART Service structure.
 * @param[in] p_nus_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t add_uuid(ble_nus_t * p_nus, 
								const ble_nus_init_t * p_nus_init, 
								uint16_t uuid, 
								ble_gatts_char_handles_t *point_handles, 
								bool nwrite, 
								bool nread,
								uint8_t *p_init_src,
								uint8_t init_len
								)
{


	ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

	// cccd_md set
	memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
	
	
    memset(&char_md, 0, sizeof(char_md));
	// set char_md
	char_md.char_props.write         = nwrite;
	char_md.char_props.read 	     = nread;
	//char_md.char_props.write_wo_resp = 1;
//	if((char_Password == uuid)||(char_Rx_Command == uuid))
//	{
//		char_md.char_props.notify = 1;
//	}
    char_md.p_char_user_desc         = NULL;
    char_md.p_char_pf                = NULL;
    char_md.p_user_desc_md           = NULL;
    char_md.p_cccd_md                = NULL;
    char_md.p_sccd_md                = NULL;

	
	// set uuid 
	ble_uuid.type = p_nus->uuid_type;
    ble_uuid.uuid = uuid;

	// att
	memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;
	
	//att char value
	memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = init_len;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_NUS_MAX_TX_CHAR_LEN;
	attr_char_value.p_value   = p_init_src;

    return sd_ble_gatts_characteristic_add(p_nus->service_handle, 
                                           &char_md,
                                           &attr_char_value,
                                           point_handles);
}


/***********************************************/

uint8_t is_ble_connected = 0;


uint8_t get_ble_connect_status(void)
{
	return is_ble_connected;
}




void ble_nus_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    if ((p_context == NULL) || (p_ble_evt == NULL))
    {
        return;
    }

    ble_nus_t * p_nus = (ble_nus_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_nus, p_ble_evt);
		
		    is_ble_connected = 1;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_nus, p_ble_evt);
		    is_ble_connected = 0;
            break;

        case BLE_GATTS_EVT_WRITE:
			on_write(p_nus, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}


uint32_t ble_nus_init(ble_nus_t * p_nus, ble_nus_init_t const * p_nus_init)
{
    uint32_t      err_code;
    ble_uuid_t    ble_uuid;
    ble_uuid128_t nus_base_uuid = NUS_BASE_UUID;

    VERIFY_PARAM_NOT_NULL(p_nus);
    VERIFY_PARAM_NOT_NULL(p_nus_init);

    // Initialize the service structure.
    p_nus->conn_handle             = BLE_CONN_HANDLE_INVALID;
    p_nus->data_handler            = p_nus_init->data_handler;
    p_nus->is_notification_enabled = false;

    /**@snippet [Adding proprietary Service to the SoftDevice] */
    // Add a custom base UUID.
    err_code = sd_ble_uuid_vs_add(&nus_base_uuid, &p_nus->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_nus->uuid_type;
    ble_uuid.uuid = BLE_UUID_NUS_SERVICE;

    // Add the service.
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_nus->service_handle);
    /**@snippet [Adding proprietary Service to the SoftDevice] */
    VERIFY_SUCCESS(err_code);

    uuid_init(p_nus, p_nus_init);

    return NRF_SUCCESS;
}


/*********************************************************
*function name: nus_send
*Para: p_nus=nus server struct;  p_str = send data; len = data len;  hand = server data handle , 
*
*      mode: mode = 1,it is passwrod notify;    
*            mode = 2,it is data handle notify  ,user data notify
************************************************************/
uint32_t nus_send(ble_nus_t * p_nus, void * p_str, uint16_t len,uint8_t mode)
{
    ble_gatts_hvx_params_t hvx_params;

    VERIFY_PARAM_NOT_NULL(p_nus);

    memset(&hvx_params, 0, sizeof(hvx_params));
	
	// mode == 2, user data  set 
	if(mode ==2 )
	{
		if ((p_nus->conn_handle == BLE_CONN_HANDLE_INVALID) || (!p_nus->is_notification_enabled))
		{
			return NRF_ERROR_INVALID_STATE;
		}
		hvx_params.handle = p_nus->Rx_hand.value_handle; // µÃµ½¾ä±ú
	}
	
    if (len > BLE_NUS_MAX_DATA_LEN)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    hvx_params.p_data = p_str;
    hvx_params.p_len  = &len;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;

    return sd_ble_gatts_hvx(p_nus->conn_handle, &hvx_params);
}


/********* send data to app****************************************************/
uint32_t ble_uuid_update(ble_nus_t * p_nus, ble_gatts_char_handles_t *hand, uint8_t *src, uint8_t len)
{
	// assert.
    if (p_nus == NULL || src == NULL || hand == NULL || len > 20 || 0 == len)
    {
        return NRF_ERROR_NULL;
    }
	uint32_t err_code = NRF_SUCCESS;
	ble_gatts_value_t gatts_value;
	memset(&gatts_value, 0, sizeof(gatts_value));
	
	// update the characteristic value.
	// update database.
	gatts_value.len     = len;
	gatts_value.offset  = 0;
	gatts_value.p_value = src;

	// set the gatts value.
	err_code = sd_ble_gatts_value_set(p_nus->conn_handle,
									  hand->value_handle,
									  &gatts_value);	

	return err_code;
}


#endif // NRF_MODULE_ENABLED(BLE_NUS)
