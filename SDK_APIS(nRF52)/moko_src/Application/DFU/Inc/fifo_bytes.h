#ifndef FIFO_BYTES_H____
#define FIFO_BYTES_H____
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

typedef struct
{
uint8_t max_fram_len;  //uint16_t
uint8_t dt_in;      //uint16_t
uint8_t dt_out;     //uint16_t
uint8_t max_fram;
uint8_t *dt_buf;
}STU_QUEUE;


void queue_dt_delete(STU_QUEUE *StuQueue,uint8_t cnt);
bool queue_dt_out(STU_QUEUE *StuQueue,uint8_t *pdata,uint8_t* data_len);
bool queue_dt_in(STU_QUEUE *StuQueue,uint8_t *datain,uint8_t len);
bool queue_is_empty(STU_QUEUE *StuQueue);
void queue_dt_init(STU_QUEUE *StuQueue,uint8_t *pbuf,uint16_t buf_len,uint8_t max_fram_len);



#endif


