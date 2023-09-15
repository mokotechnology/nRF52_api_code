#include "bsp.h"
#include "nrf_sdm.h"

void HardFault_Handler(void)
{
	APP_ERROR_CHECK(0xaaaa);
}

void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{

//    switch (id)
//    {
//#if defined(SOFTDEVICE_PRESENT) && SOFTDEVICE_PRESENT
//        case NRF_FAULT_ID_SD_ASSERT:
//            BLE_RTT("SOFTDEVICE: ASSERTION FAILED");
//            break;
//        case NRF_FAULT_ID_APP_MEMACC:
//            BLE_RTT("SOFTDEVICE: INVALID MEMORY ACCESS");
//            break;
//#endif
//        case NRF_FAULT_ID_SDK_ASSERT:
//        {
//            assert_info_t * p_info = (assert_info_t *)info;
//            BLE_RTT("ASSERTION FAILED at %s:%u",
//                          p_info->p_file_name,
//                          p_info->line_num);
//            break;
//        }
//        case NRF_FAULT_ID_SDK_ERROR:
//        {
//            error_info_t * p_info = (error_info_t *)info;
//            BLE_RTT("ERROR %u [%s] at %s:%u",
//                          p_info->err_code,
//                          nrf_strerror_get(p_info->err_code),
//                          p_info->p_file_name,
//                          p_info->line_num);
//            break;
//        }
//        default:
//            BLE_RTT("UNKNOWN FAULT at 0x%08X", pc);
//            break;
//    }



}

void delay_us(uint32_t us)
{
    uint32_t i,j;
    for(j=0; j<us; j++)
    {
        for(i=0; i<10; i++)
        {
            __ASM("NOP");
        }
        __ASM("NOP");
        __ASM("NOP");
        __ASM("NOP");
        __ASM("NOP");
        __ASM("NOP");
        __ASM("NOP");
    }
}
void delay_ms(uint32_t ms)
{
    uint32_t j;
    for(j=0; j<ms; j++)
        delay_us(880);
}















 
//static void button_evt_handler(uint8_t pin_no, uint8_t button_action)
//{
//#if 1
//    if (button_action == APP_BUTTON_PUSH && pin_no == KEY_PIN)
//    {
////        nrf_ble_es_on_start_connectable_advertising();
//				LED_TOG();
//    }
//#endif
//}



//static void button_init(void)
//{
//    uint32_t              err_code;
//    const uint8_t           buttons_cnt  = 1;
//    static app_button_cfg_t buttons_cfgs =
//    {
//        .pin_no         = KEY_PIN,
//        .active_state   = APP_BUTTON_ACTIVE_LOW,
//        .pull_cfg       = NRF_GPIO_PIN_PULLUP,
//        .button_handler = button_evt_handler
//    };

//    err_code = app_button_init(&buttons_cfgs, buttons_cnt, APP_TIMER_TICKS(20));
//    APP_ERROR_CHECK(err_code);

//    err_code = app_button_enable();
//    APP_ERROR_CHECK(err_code);
//}
//void gpio_init(void)
//{
//	uint32_t err_code;
//	err_code = app_timer_init();
//	APP_ERROR_CHECK(err_code);
//	nrf_gpio_cfg_output(LED_PIN);
//	LED_OFF();

//	button_init();

//	LED_ON();

//}


