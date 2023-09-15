#include "ble_fifo.h"





static bool BleQueueflg=false;
static uint8_t BleIn=0,BleOut=0;
static uint8_t BleBuf[BLE_BUF_SIZE][21];

bool BleQueueIn(uint8_t*datain,uint8_t len)
{

		memcpy(&BleBuf[BleIn][1],datain,len);
		BleBuf[BleIn][0]=len;
		BleIn++;
		BleIn%=BLE_BUF_SIZE;
		BleQueueflg=true;
		return true;


}

void BleQueueAllOut(void)
{
	BleOut=0;
	BleIn=0;
	BleQueueflg = false; 

}
bool BleQueueOut(uint8_t*dataout,uint8_t* len)
{

	if(BleQueueflg!=true) 
	{
		return false;
	}
	else
	{
		*len=BleBuf[BleOut][0];
		memcpy(dataout,&BleBuf[BleOut][1],*len);
		BleOut++;
		BleOut %= BLE_BUF_SIZE; 
		if(BleOut==BleIn)
		{
		BleQueueflg = false; 
		}
		return true;
	}
}


