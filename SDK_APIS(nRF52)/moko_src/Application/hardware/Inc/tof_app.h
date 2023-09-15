#ifndef TOF_APP_H
#define TOF_APP_H

#include "stdint.h"
#include "string.h"


#define CHANGE_TOFMODE_DELAY    5

enum
{
    STORT_RANGE_MODE = 1,
    LONG_RANGE_MODE = 2,
};

void Task_Change_TofMode(uint8_t tick);
void reset_cur_tof_interval(void);
void task_tof_interval_tick(uint8_t tick);

#endif