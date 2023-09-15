#ifndef lis3dh_app_h_
#define  lis3dh_app_h_


#include <stdint.h>
#include <string.h>
#include "stdbool.h"





bool  lis3dh_init(void);
void  task_read_3dh(void);
bool  get_3dh_sensor_state(void);

#endif
