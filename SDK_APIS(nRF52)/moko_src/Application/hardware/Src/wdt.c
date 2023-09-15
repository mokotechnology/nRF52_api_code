#include "wdt.h"


#ifdef ENLABLE_DOG
static bool B_ReadWdtRunStatues(void)
{
    if(NRF_WDT->RUNSTATUS)
        return true;
    else
        return false;
}
#endif


/***********************************************
*@function:FeedWdog
*@description:Feed   Wdog 
*******************************************/
void Feed_WacthDog(void)
{
#ifdef ENLABLE_DOG
    NRF_WDT->RREN=1;
    NRF_WDT->RR[0]=0x6E524635;
    NRF_WDT->RREN=0;
#endif
}

/***********************************************
*@function:WdtInit
*@description:init wacth dog
*******************************************/
void watchdog_init(uint32_t TimeOutMs)
{
#ifdef ENLABLE_DOG
    NRF_WDT->TASKS_START=false;
    NRF_WDT->CRV=32768*TimeOutMs/1000-1;
    NRF_WDT->CONFIG=0;
    NRF_WDT->CONFIG|=0x01;//Keep the watchdog running while the CPU is sleeping.
    NRF_WDT->CONFIG|=0x08;//Keep the watchdog running while the CPU is halted by the debugger.
	
    //NRF_WDT->CONFIG&=0xf7;//Pause watchdog while the CPU is halted by the debugger.
    //NRF_WDT->INTENSET=WDT_INTENSET_TIMEOUT_Set;//???????
    NRF_WDT->TASKS_START=true;
		
    while(false==B_ReadWdtRunStatues());
    Feed_WacthDog();
#endif
}






//nrf_drv_wdt_channel_id    m_channel_id;


//void wdt_configure(void)
//{
//    uint32_t err_code = NRF_SUCCESS;
//    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;/**< WDT will be paused when CPU is in SLEEP or HALT mode. */

//    err_code = nrf_drv_wdt_init(&config, NULL);
//    APP_ERROR_CHECK(err_code);
//    err_code = nrf_drv_wdt_channel_alloc(&m_channel_id);
//    APP_ERROR_CHECK(err_code);
//    nrf_drv_wdt_enable();
//}


//void wdt_feed(void)
//{
//    nrf_drv_wdt_channel_feed(m_channel_id);
//}



