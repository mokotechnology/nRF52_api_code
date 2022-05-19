 
#ifndef SIMPLE_UART_H__
#define SIMPLE_UART_H__


#include <stdbool.h>
#include <stdint.h>





#define RX_PIN         8 //
#define TX_PIN         9 //


//#define HWFC           true

#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */




uint8_t get_sensor_type(void);

void start_uart(bool stt_flg);
void check_start_uart(void);

void simple_uart_init(void);
void simple_uart_uinit(void);
void uart_send(uint8_t *str,uint16_t len);


void task_uart_manage(void);
void task_check_uart_timeout(void);


void  task_process_uart_data(void);

#endif
