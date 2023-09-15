#include "tof_app.h"
#include "vl53lx_app.h"
#include "user_flash.h"
#include "bsp.h"
#include "key.h"

uint8_t reset_cur_tof_interval_flg = 0;
void reset_cur_tof_interval(void)
{
    reset_cur_tof_interval_flg = 1;
}

/*
    start measure distance every tof_interval time
*/
void task_tof_interval_tick(uint8_t tick)
{
    static uint16_t cur_tof_interval=0;

    tof_StopMeasurement();

    if(get_device_sta()==false)return;
    
    if(reset_cur_tof_interval_flg)
    {
        reset_cur_tof_interval_flg = 0;
        cur_tof_interval = 0;
    }

    if(cur_tof_interval>tick)
    {
        cur_tof_interval-=tick;
    }
    else if(cur_tof_interval<=tick)
    {
        cur_tof_interval = 0;
    }

    //BLE_RTT("cur_tof_interval %d \r\n",cur_tof_interval);
    if(cur_tof_interval==0)
    {
        start_tof_measurement();
        cur_tof_interval = StuHis.tof_interval;
    }
}

void Toggle_TofMode(void)
{
    if(StuHis.tof_mode==LONG_RANGE_MODE)
    {
        StuHis.tof_mode = STORT_RANGE_MODE;
        BLE_RTT("Tof mode change STORT_RANGE_MODE\r\n");
    }
    else
    {
        StuHis.tof_mode = LONG_RANGE_MODE;
        BLE_RTT("Tof mode change LONG_RANGE_MODE\r\n");
    }
}

/*
    Toggle the Tof Mode every 5 seconds
*/
void Task_Change_TofMode(uint8_t tick)
{
    static uint8_t tofmode_change_tick = 0;
    if(get_device_sta()==false)return;

    if(tofmode_change_tick < CHANGE_TOFMODE_DELAY)
    {
        tofmode_change_tick+=tick;
    }
    else
    {
        tofmode_change_tick = 0;

        Toggle_TofMode();

        set_vl53lx_config(StuHis.tof_mode);

    }
    
}