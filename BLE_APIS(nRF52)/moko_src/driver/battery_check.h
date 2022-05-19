#ifndef battery_check_h_
#define battery_check_h_


#include <stdint.h>



#define Read_battery_30s     30
#define Read_battery_30min   180//(30*60)



uint16_t get_battery_valu(void);
uint16_t get_battery_level(void);


void task_read_battery(void);
#endif

