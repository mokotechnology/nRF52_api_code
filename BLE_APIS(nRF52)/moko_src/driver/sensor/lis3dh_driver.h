#ifndef lis3dh_driver_h_
#define lis3dh_driver_h_



#include <stdint.h>
#include <string.h>
#include "stdbool.h"







#include <stdint.h>
#include <string.h>
#include "stdbool.h"








#define BIT(x) ( (x) )


//Register Definition
#define LIS3DH_WHO_AM_I    0x0F  // device identification register
#define LIS3DH_READBIT                (0x80)

#define LIS3DH_FIFO_SRC_REG           0x2F

// CONTROL REGISTER 1
#define LIS3DH_CTRL_REG1   0x20
#define LIS3DH_ODR_BIT     BIT(4)
#define LIS3DH_LPEN        BIT(3)
#define LIS3DH_ZEN         BIT(2)
#define LIS3DH_YEN         BIT(1)
#define LIS3DH_XEN         BIT(0)

//CONTROL REGISTER 2
#define LIS3DH_CTRL_REG2   0x21
#define LIS3DH_HPM         BIT(6)
#define LIS3DH_HPCF        BIT(4)
#define LIS3DH_FDS         BIT(3)
#define LIS3DH_HPCLICK     BIT(2)
#define LIS3DH_HPIS2       BIT(1)
#define LIS3DH_HPIS1       BIT(0)

//CONTROL REGISTER 3
#define LIS3DH_CTRL_REG3   0x22
#define LIS3DH_I1_CLICK    BIT(7)
#define LIS3DH_I1_AOI1     BIT(6)
#define LIS3DH_I1_AOI2     BIT(5)
#define LIS3DH_I1_DRDY1    BIT(4)
#define LIS3DH_I1_DRDY2    BIT(3)
#define LIS3DH_I1_WTM      BIT(2)
#define LIS3DH_I1_ORUN     BIT(1)

//CONTROL REG3 bit mask
#define LIS3DH_CLICK_ON_PIN_INT1_ENABLE                0x80
#define LIS3DH_CLICK_ON_PIN_INT1_DISABLE               0x00
#define LIS3DH_I1_INT1_ON_PIN_INT1_ENABLE              0x40
#define LIS3DH_I1_INT1_ON_PIN_INT1_DISABLE             0x00
#define LIS3DH_I1_INT2_ON_PIN_INT1_ENABLE              0x20
#define LIS3DH_I1_INT2_ON_PIN_INT1_DISABLE             0x00
#define LIS3DH_I1_DRDY1_ON_INT1_ENABLE                 0x10
#define LIS3DH_I1_DRDY1_ON_INT1_DISABLE                0x00
#define LIS3DH_I1_DRDY2_ON_INT1_ENABLE                 0x08
#define LIS3DH_I1_DRDY2_ON_INT1_DISABLE                0x00
#define LIS3DH_WTM_ON_INT1_ENABLE                      0x04
#define LIS3DH_WTM_ON_INT1_DISABLE                     0x00
#define LIS3DH_INT1_OVERRUN_ENABLE                     0x02
#define LIS3DH_INT1_OVERRUN_DISABLE                    0x00

//CONTROL REGISTER 4
#define LIS3DH_CTRL_REG4    0x23
#define LIS3DH_CTRL_REG4    0x23
#define LIS3DH_BDU     BIT(7)
#define LIS3DH_BLE     BIT(6)
#define LIS3DH_FS     BIT(4)
#define LIS3DH_HR     BIT(3)
#define LIS3DH_ST      BIT(1)
#define LIS3DH_SIM     BIT(0)



#define MEMS_SET                                        0x01
#define MEMS_RESET                                      0x00


//FIFO CONTROL REGISTER
#define LIS3DH_FIFO_CTRL_REG                           0x2E
#define LIS3DH_FM                                      BIT(6)
#define LIS3DH_TR                                      BIT(5)
#define LIS3DH_FTH                                     BIT(0)

#define LIS3DH_CTRL_REG5    0x24
#define LIS3DH_BOOT                                    BIT(7)
#define LIS3DH_FIFO_EN                                 BIT(6)
#define LIS3DH_LIR_INT1                                BIT(3)
#define LIS3DH_D4D_INT1                                BIT(2)


//OUTPUT REGISTER
#define LIS3DH_OUT_X_L     0x28
#define LIS3DH_OUT_X_H     0x29
#define LIS3DH_OUT_Y_L     0x2A
#define LIS3DH_OUT_Y_H     0x2B
#define LIS3DH_OUT_Z_L     0x2C
#define LIS3DH_OUT_Z_H     0x2D

//AUX REGISTER
#define LIS3DH_OUT_1_L     0x08
#define LIS3DH_OUT_1_H     0x09
#define LIS3DH_OUT_2_L     0x0A
#define LIS3DH_OUT_2_H     0x0B
#define LIS3DH_OUT_3_L     0x0C
#define LIS3DH_OUT_3_H     0x0D

//INT_Reg
#define INT1_CFG			0x30
#define INT1_SRC			0x31
#define INT1_THS			0x32
#define INT1_DURATION		0x33

#define MOVE_MODE			0x2a

extern uint32_t acc_sum;

// lis3dh mode
typedef enum {
    LIS3DH_POWER_DOWN                    =  0xFF,
    LIS3DH_LOW_POWER    =  0x00,
    LIS3DH_NORMAL   =  0x01,
	LIS3DH_HIGH     =  0x02
} LIS3DH_Mode_t;


// x,y,z轴使能
typedef enum {
    LIS3DH_X_ENABLE                      =               0x01,
    LIS3DH_X_DISABLE                     =               0x00,
    LIS3DH_Y_ENABLE                      =               0x02,
    LIS3DH_Y_DISABLE                     =               0x00,
    LIS3DH_Z_ENABLE                      =               0x04,
    LIS3DH_Z_DISABLE                     =               0x00
} LIS3DH_AXISenable_t;


/// lis3dh  频率
typedef enum {
	LIS3DH_PWR_DOWN		= 0x00,
    LIS3DH_ODR_1Hz          =  0x01,
    LIS3DH_ODR_10Hz          =  0x02,
    LIS3DH_ODR_25Hz          =  0x03,
    LIS3DH_ODR_50Hz          =  0x04,
    LIS3DH_ODR_100Hz          =  0x05,
    LIS3DH_ODR_200Hz          =  0x06,
    LIS3DH_ODR_400Hz          =  0x07,
    LIS3DH_ODR_1620Hz_LP          =  0x08,
    LIS3DH_ODR_1344Hz_NP_5367HZ_LP       =  0x09
} LIS3DH_ODR_t;


//  g 重量+2g,+4g,
typedef enum {
    LIS3DH_FULLSCALE_2                   =               0x00,
    LIS3DH_FULLSCALE_4                   =               0x01,
    LIS3DH_FULLSCALE_8                   =               0x02,
    LIS3DH_FULLSCALE_16                  =               0x03
} LIS3DH_Fullscale_t;


// 设置高位或低位开始mode
typedef enum {
    LIS3DH_BLE_LSB   =  0x00,
    LIS3DH_BLE_MSB   =  0x01
} LIS3DH_Endianess_t;


typedef enum {
    LIS3DH_FIFO_BYPASS_MODE              =               0x00,
    LIS3DH_FIFO_MODE                     =               0x01,
    LIS3DH_FIFO_STREAM_MODE              =               0x02,
    LIS3DH_FIFO_TRIGGER_MODE             =               0x03,
    LIS3DH_FIFO_DISABLE                  =               0x04
} LIS3DH_FifoMode_t;




typedef enum {
    MEMS_SUCCESS    =  0x01,
    MEMS_ERROR    =  0x00
} status_t;

typedef enum {
    MEMS_ENABLE    =  0x01,
    MEMS_DISABLE    =  0x00
} State_t;


#pragma pack(1)
typedef struct
{
    int16_t AXIS_X;
    int16_t AXIS_Y;
    int16_t AXIS_Z;
}
AxesRaw_t;

#pragma pack()



bool LIS3DH_ReadReg(uint8_t Reg, uint8_t* Data);// note!!! use this function for your project.
bool LIS3DH_WriteReg(uint8_t WriteAddr, uint8_t Data);
bool LIS3DH_SetMode(LIS3DH_Mode_t md);
status_t LIS3DH_SetAxis(uint8_t axis);
status_t LIS3DH_SetODR(LIS3DH_ODR_t ov);
status_t LIS3DH_SetFullScale(LIS3DH_Fullscale_t fs);
status_t LIS3DH_SetBLE(LIS3DH_Endianess_t ble);
status_t LIS3DH_FIFOModeEnable(LIS3DH_FifoMode_t fm);
status_t LIS3DH_GetAccAxesRaw(AxesRaw_t* buff);

status_t LIS3DH_SetWaterMark(uint8_t wtm);
status_t LIS3DH_SetBDU(State_t bdu);
void LIS3DH_GetTimes(uint8_t *times);

status_t LIS3DH_SetWaterMark(uint8_t wtm);
status_t LIS3DH_SetBDU(State_t bdu);
status_t LIS3DH_SetInt1Pin(uint8_t pinConf);
status_t LIS3DH_SetFilterDataSel(State_t state);
status_t LIS3DH_HPFAOI1Enable(State_t hpfe);
status_t LIS3DH_Int1LatchEnable(State_t latch);



#endif

