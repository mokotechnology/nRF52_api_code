#ifndef BLE_FIFO_H__
#define BLE_FIFO_H__

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#define BLE_BUF_SIZE 15



//#define NOT_USE_SAME


bool BleQueueIn(uint8_t*datain,uint8_t len);

bool BleQueueOut(uint8_t*dataout,uint8_t* len);

void BleQueueAllOut(void);


//uint8_t  *TxBufDataOut(uint8_t *len);
//bool TxBleDataIn(uint8_t *buf,uint8_t len);
//uint8_t TxBleLen(void);
#endif


