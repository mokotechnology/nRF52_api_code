#ifndef uart_commd_h_
#define uart_commd_h_


#include "string.h"
#include "stdint.h"
#include "stdbool.h"









void task_uart_write_flash(void);

void analyze_uart_data(uint8_t cmd,uint8_t*datas,uint8_t len);
void uart_send_cmd(uint8_t cmd,uint8_t*datas,uint8_t dlen);
#endif

