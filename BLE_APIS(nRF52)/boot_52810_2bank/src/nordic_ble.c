#include "nordic_ble.h"
#include "in_flash_manage.h"
#include "nrf_sdh_soc.h"
#include "nrf_dfu.h"




NRF_BLE_GATT_DEF(m_gatt);                                   //!< GATT module instance.


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xBEEF, line_num, p_file_name);
}



static void ble_evt_handler(ble_evt_t const * p_ble_evt)
{
    ret_code_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(p_ble_evt->evt.common_evt.conn_handle,
                                                   BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP,
                                                   NULL,
                                                   NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(p_ble_evt->evt.common_evt.conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_CONNECTED:

            break;
      
        case BLE_GAP_EVT_DISCONNECTED:
				case BLE_GAP_EVT_TIMEOUT:
            start_connect_adv();
            break;

#ifndef S140
        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        }
        break;
#endif

        default:
            // No implementation needed.
            break;
    }
}

void nrf_ble_es_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
	ble_evt_handler(p_ble_evt);
//	eddystone_service_on_ble(p_ble_evt);
	dfu_transport_callby_irq((ble_evt_t *)p_ble_evt);
    
    
    
    
}

//    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, nrf_ble_es_on_ble_evt, NULL);
//NRF_SDH_SOC_OBSERVER(m_sys_obs, 0, call_by_sys_flash_manage, NULL);
//NRF_SDH_SOC_OBSERVER(m_sys_obs1, 0, call_by_sys, NULL);//DFU的回调函数
/**@brief Function for the GAP initialization.
*
* @details This function will set up all the necessary GAP (Generic Access Profile) parameters of
*          the device. It also sets the permissions and appearance.
*/
#define MIN_CONN_INTERVAL             MSEC_TO_UNITS(7.5, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL              MSEC_TO_UNITS(50, UNIT_1_25_MS)           /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                  0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT               MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory time-out (4 seconds). */
static void gap_params_init(void)
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;
    uint8_t                 device_name[] = DEVICE_NAME;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          device_name,
                                          strlen((const char *)device_name));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);

    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
#include "nrf_sdm.h"
static void ble_stack_init(void)
{
    ret_code_t err_code;

    

        err_code = nrf_dfu_mbr_init_sd();
        APP_ERROR_CHECK(err_code);
    


    err_code = sd_softdevice_vector_table_base_set(BOOT_ADDR);
    APP_ERROR_CHECK(err_code);
    
    
    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_app_ram_start_get(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Overwrite some of the default configurations for the BLE stack.
    ble_cfg_t ble_cfg;

    // Configure the maximum number of connections.
    memset(&ble_cfg, 0, sizeof(ble_cfg));
    ble_cfg.gap_cfg.role_count_cfg.periph_role_count  = 1;
#if !defined (S112)
    ble_cfg.gap_cfg.role_count_cfg.central_role_count = 0;
    ble_cfg.gap_cfg.role_count_cfg.central_sec_count  = 0;
#endif // !defined (S112)
    err_code = sd_ble_cfg_set(BLE_GAP_CFG_ROLE_COUNT, &ble_cfg, ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, nrf_ble_es_on_ble_evt, NULL);
}


/**@brief Function for initializing the Connection Parameters module.
 */

#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(1000)  /**< Time from initiating an event (connect or start of notification) to the first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(2000) /**< Time between each call to sd_ble_gap_conn_param_update after the first (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT   4                                          /**< Number of attempts before giving up the connection parameter negotiation. */

static void conn_params_init(void)
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);

}


/**@brief Function for doing power management.
 */
void power_manage(void)
{
    ret_code_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

/*
datas指向的内存必须一直存在
*/
static void start_adv(uint8_t *datas,uint8_t len,bool connectable)
{
    ble_gap_adv_params_t connectable_adv_params;
    ble_advdata_t adv_data;
    ret_code_t    err_code;
    ble_advdata_service_data_t service_data; // Structure to hold Service Data.

    service_data.service_uuid = 0xff10; // Eddystone UUID to allow discoverability on iOS devices.
    if(len> (31-7))
    {
        len=31-7;
    }
    service_data.data.size=len;
    service_data.data.p_data=datas;


    memset(&adv_data, 0, sizeof(ble_advdata_t));
    adv_data.name_type               = BLE_ADVDATA_NO_NAME;
    adv_data.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    adv_data.p_service_data_array    = &service_data;
    adv_data.service_data_count      = 1;

    err_code = ble_advdata_set(&adv_data, NULL);
    APP_ERROR_CHECK(err_code);


    memset(&connectable_adv_params, 0, sizeof(ble_gap_adv_params_t));
    if(true== connectable)
        connectable_adv_params.type = BLE_GAP_ADV_TYPE_ADV_IND;
    else
        connectable_adv_params.type = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
    connectable_adv_params.p_peer_addr = NULL; // Undirected advertisement.
    connectable_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    connectable_adv_params.interval    = 1000;
    connectable_adv_params.timeout     =  60;

    err_code = sd_ble_gap_adv_start(&connectable_adv_params, BLE_CONN_CFG_TAG_DEFAULT);
    if (err_code != NRF_ERROR_BUSY && err_code != NRF_SUCCESS)
    {
        APP_ERROR_CHECK(err_code);
    }



}


void start_connect_adv(void)
{
    ble_gap_addr_t addr;
    static uint8_t  buf[31]= {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};
//    uint8_t len=0,i;
//    uint8_t device_name[] = DEVICE_NAME;

//    buf[len++]=StuHis.adv_interval;
//    buf[len++]=StuHis.tx_power;
//    sd_ble_gap_addr_get(&addr);
//    for(i=0; i<6; i++)
//    {
//        buf[len++]=addr.addr[i];
//    }
//    buf[len++] = soft_version;
//    buf[len++] = StuHis.connect_able;
////    buf[len++] = GetBattery();
//    for(i=0; i<strlen((const char*)device_name); i++)
//    {
//        buf[len++]=device_name[i];
//    }


    start_adv(buf,2,true);
}


NRF_SDH_SOC_OBSERVER(m_sys_obs, 0, call_by_sys_flash_manage, NULL);
NRF_SDH_SOC_OBSERVER(m_sys_obs1, 0, call_by_sys, NULL);//DFU的回调函数

void ble_init(void)
{

    //APP_SCHED_INIT(APP_TIMER_SCHED_EVENT_DATA_SIZE, 5);
    ble_stack_init();
    gap_params_init();
    gatt_init();
    conn_params_init();
    //InitAdc();
    //GetBattery();
    //InitHis();
    //eddystone_service_init();
	dfu_transport_init();
   

}


