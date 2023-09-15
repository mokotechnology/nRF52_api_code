/**
 * @file 53L3A2.c
 *
 * implement X-NUCLEO-53L3A2 Nucleo BSP
 */


#include <string.h>

#include "vl53lx_api.h"
#include "vl53lx_app.h"
#include "ble_init.h"
#include "vl53l0x_iic.h"
#include "vl53lx_wait.h"
#include "wdt.h"
#include "gpio_init.h"
#include "VL53L3CX_ULP_api.h"
#include "user_flash.h"

#include "nrf_delay.h"
#include "bsp.h"

VL53LX_Dev_t                   dev;
VL53LX_DEV                     Dev = &dev;
uint8_t ToFSensor = 1;
int16_t vl53lx_distance = 0xFFFF;
uint8_t tof_sensor_type=0;
static uint8_t tof_int_flg=0;

#define XSHUT_CLR	NRF_GPIO->OUTCLR = 1<<XSHUT_IO
#define XSHUT_SET	NRF_GPIO->OUTSET = 1<<XSHUT_IO
/* when not customized by application define dummy one */
#ifndef XNUCLEO53L3A2_GetI2cBus
/**
 * macro that can be overloaded by user to enforce i2c sharing in RTOS context
 */
#define XNUCLEO53L3A2_GetI2cBus(...) (void)0
#endif

#ifndef XNUCLEO53L3A2_PutI2cBus
/** macro can be overloaded by user to enforce i2c sharing in RTOS context
 */
#   define XNUCLEO53L3A2_PutI2cBus(...) (void)0
#endif

/**
 *@mainpage
 *@image html VL53L3A2_board.jpg "X-NUCLEO-53L3A2 Expansion Board"

  @section Quick BSP links
  - @ref XNUCLEO53L3A2_Interface
  - Board information and configuration @ref XNUCLEO53L3A2_Board
  - Go to Modules panes


  @section XNUCLEO53L3A2_usage How to use X-NUCLEO-53L3A2 BSP ?
  ## Multiple family project ##
   X-NUCLEO-53L3A2 BSP supports multiple STM32 families through unique "stm32xxx_hal.h".
   This file redirects to the targeted stm32 HAL.
   @note This file can also be used to place any customization defines

  @section XNUCLEO53L3A2_conf  How to configure X-NUCLEO-53L3A2 BSP ?
  ## configuration ##
    see @ref VL53L3A2_config

  ### MSP implementation ###
   @li @ref MSP_implement
     @li @ref MSP_implement_common
     @li @ref MSP_implement_STM32x4

*/


/**
 * @defgroup XNUCLEO53L3A2_Board X-NUCLEO-53L3A2 Expansion Board
 * @{
 */

/**
 * @defgroup VL53L3A2_GPIO1_MAP    VL53L3A2 Sensor to MCU interrupt mapping
 *
 * # GPIO mapping ##
 * Various options can be used to map the 3 VL53L0X interrupt lines to MCU. By default, the expansion board is configured in
 * Center on-board vl53l0x mode which means only the center device can be used in interrupt mode. To use left and/or right devices
 * in interrupt mode, it is necessary to adapt the board configuration as described below.
 * ## One interrupt line shared by all sensors ##
 * All VL53L0x GPIO1 pins (open collector outputs) are connected together on the expansion board
 * and level shifter input drives single shared interrupt line to MCU.\n
 * Solder options to operate in this mode:
 * @li U7 and U8 are fitted  (connect GPIO all together before level shifter)
 * @li U10, U11, U15 and U18 are not soldered. (disconnect all level shifted option to arduino connector)
 * @li U14/U17 to select  PA4/PC1  => EXTI4_15_IRQn / EXTI0_1_IRQn (final selection option)
 *
 * see @a #VL53L3A2_GPIO1_C_OPTION for interrupt line selection
 *
 * @note requires @a #VL53L3A2_GPIO1_SHARED to be set (to 1)
 *
 * ##  One interrupt per sensor  ##
 * To use one interrupt per device :\n
 * @li Do not define @a #VL53L3A2_GPIO1_SHARED  or set to 0
 * @li U7 and U8 must not be soldered (disconnect L and R GPIO from C before level shifter)
 *
 * ### Center on-board vl53l0x  ###
 * @li U14 (fitted by default)  CN8#3    PA4
 * @li U17 (*option)         	CN8#5    PC1
 *
 * see @ref VL53L3A2_GPIO1_C_OPTION
 *
 * ### Left satellite ###
 * @li U10 (not fitted by default)  CN5#2    PC7
 * @li U11 (*option)         		CN5#1    PA9
 *
 * see @a #VL53L3A2_GPIO1_L_OPTION
 *
 * ### Right satellite ###
 * @li U18 (not fitted by default)  CN9#3    PA10
 * @li U15 (*option)       			CN9#5    PB5

 *
 * see @a #VL53L3A2_GPIO1_R_OPTION
 *
 * ### Interrupt vectors F401 and L476 ###
 * @li Center PA4 / PC1  => Extit4 / Exti 1
 * @li Left PC7 / PA9 => Extit9_5 / Extit9_5
 * @li Right PA10 / PB5 => Extit15_10 / Extit9_5
 *
 * @warning When selecting alternate option for both sensor L/R interrupt line will shared same vector
 *
 * ### L053 ###
 * @li Center PA4 / PC1  => EXTI4_15_IRQn / EXTI0_1_IRQn
 * @li Left PC7 / PA9  => EXTI4_15_IRQn / EXTI4_15_IRQn
 * @li Right PA10 /  PB5 => EXTI4_15_IRQn / EXTI4_15_IRQn
 *
 * @warning R and L have shared vector use, and will also share C vector if alternate option is selected for C
 */

/**
 * @defgroup  XNUCLEO53L3A2_I2CExpanders I2C expender mapping
 * ## I2C Expanders address and i/o ##
 * Digit 1 2 3 and 4 below are numbered left to right "1234" on see on display
 * ### Expander 0 ###
 * U21 A[2-0]= 001 => i2c address[7..1] 0x43 ([7..0] 0x86 )
 * @li Digit#1  gpio 0 to 6
 * @li Digit#2  gpio 7 to 13
 * @li xshut_l  gpio 14
 * @li xshut_r  gpio 15
 *
 * ### Expander 1 ###
 * U19 A[2-0]= 000 => i2c address[7..1] 0x42 ([7..0] 0x84 )
 * @li Digit#3  gpio 0 to 6
 * @li Digit#4  gpio 7 to 13
 * @li PB1      gpio 14
 * @li xshut    gpio 15
 *
 * @note The 0/1  assignment is "digit" order logical don't look for any sense as  per device address or part number
 * @{
 */

/**
 * Expander 0 i2c address[7..0] format
 */
#define I2cExpAddr0 0x52//((int)(0x43*2))
/**
 * Expander 1 i2c address[7..0] format
 */
#define I2cExpAddr1 0x52//((int)(0x42*2))
/** @} XNUCLEO53L3A2_I2CExpanders*/


/**
 * GPIO monitor pin state register
 * 16 bit register LSB at lowest offset (little endian)
 */
#define GPMR    0x10
/**
 * STMPE1600 GPIO set pin state register
 * 16 bit register LSB at lowest offset (little endian)
 */
#define GPSR    0x12
/**
 * STMPE1600 GPIO set pin direction register
 * 16 bit register LSB at lowest offset
 */
#define GPDR    0x14


/** @} */ /* defgroup  XNUCLEO53L3A2_Board */


/****************************************************
 *@defgroup  XNUCLEO53L3A2_globals
 *@{
 */

/**
 *  i2c handle to be  use of all i2c access
 * end user shall provide it to
 * can be @a XNUCLEO53L3A2_I2C1Configure() @sa XNUCLEO53L3A2_usage
 * @warning do not use any XNUCLEO53L3A2_xxx prior to a first init with valid i2c handle
 */



/**
 * cache the full set of expanded GPIO values to avoid i2c reading
 */
static union CurIOVal_u {
    uint8_t bytes[4];   /*!<  4 bytes array i/o view */
    uint32_t u32;       /*!<  single dword i/o view */
}
/** cache the extended IO values */
CurIOVal;

/**
 * lookup table for for digit  to bit position in @a CurIOVal u32
 */
static int  DisplayBitPos[4]={0, 7, 16, 16+7};

/** @} XNUCLEO53L3A2_globals*/

/* Forward definition of private function */

static int _ExpanderRd(int I2cExpAddr, int index, uint8_t *data, int n_data);
static int _ExpanderWR(int I2cExpAddr, int index, uint8_t *data, int n_data);
static int _ExpandersSetAllIO(void);







int XNUCLEO53L3A2_Init(void) {
    int status;
    uint8_t ExpanderData[2];
    uint8_t byteData;
    uint16_t wordData;
    
    IIC_Init();
    IIC_Start();
    status = _ExpanderRd( I2cExpAddr0, 0, ExpanderData, 2);
    if (status != 0 || ExpanderData[0] != 0x00 || ExpanderData[1] != 0x16) {
        XNUCLEO53L3A2_ErrLog("I2C Expander @0x%02X not detected",(int)I2cExpAddr0 );
//        goto done_err;

    }
    status = _ExpanderRd( I2cExpAddr1, 0, ExpanderData, 2);
    if (status != 0 || ExpanderData[0] != 0x00 || ExpanderData[1] != 0x16) {
        XNUCLEO53L3A2_ErrLog("I2C Expander @0x%02X not detected",(int)I2cExpAddr1);
//        goto done_err;
    }

    CurIOVal.u32=0x0;
    /* setup expender   i/o direction  all output but exp1 bit 14*/
    ExpanderData[0] = 0xFF;
    ExpanderData[1] = 0xFF;
    status = _ExpanderWR(I2cExpAddr0, GPDR, ExpanderData, 2);
    if (status) {
        XNUCLEO53L3A2_ErrLog("Set Expander @0x%02X DR", I2cExpAddr0);
//        goto done_err;
    }
    ExpanderData[0] = 0xFF;
    ExpanderData[1] = 0xBF; // all but bit 14-15 that is pb1 and xhurt
    status = _ExpanderWR(I2cExpAddr1, GPDR, ExpanderData, 2);
    if (status) {
        XNUCLEO53L3A2_ErrLog("Set Expander @0x%02X DR", I2cExpAddr1);
//        goto done_err;
    }
    /* shut down all segment and all device */
    CurIOVal.u32=0x7F + (0x7F<<7) + (0x7F<<16)+(0x7F<<(16+7));
    status= _ExpandersSetAllIO();
    if( status ){
        XNUCLEO53L3A2_ErrLog("Set initial i/o ");
    }

      Dev->i2c_slave_address = 0x52;
  
  /* Allow to select the sensor to be used, multi-sensor is not managed in this example.
  Only when use use the Left ToF in interrupt mode,  solder the U7 on the X-Nucleo-53L3A2 board 
  Only when use the Right ToF in interrupt mode, solder the U7 on the X-Nucleo-53L3A2 board
  See "Solder drop configurations" in the X-Nucleo-53L3A2 User Manual for more details */
  ToFSensor = 1; // Select ToFSensor: 0=Left, 1=Center, 2=Right
  status = XNUCLEO53L3A2_ResetId(ToFSensor, 0); // Reset ToF sensor
  nrf_delay_ms(2);
  status = XNUCLEO53L3A2_ResetId(ToFSensor, 1); // Reset ToF sensor
  nrf_delay_ms(2);
  
  VL53LX_RdByte(Dev, 0x010F, &byteData);
  BLE_RTT("VL53LX Model_ID: %02X\n\r", byteData);
  VL53LX_RdByte(Dev, 0x0110, &byteData);
  BLE_RTT("VL53LX Module_Type: %02X\n\r", byteData);
  VL53LX_RdWord(Dev, 0x010F, &wordData);
  BLE_RTT("VL53LX: %02X\n\r", wordData);

done_err:
    return status;
}


int XNUCLEO53L3A2_GetPB1(int *state) {
    int status;
    uint8_t  PortValue;
    status= _ExpanderRd(I2cExpAddr1, GPMR+1, &PortValue,1);
    if( status == 0){
        if( PortValue&=0x40 )
            PortValue=1;
        else
            PortValue=0;
    }
    else{
        XNUCLEO53L3A2_ErrLog("i/o error");
    }
    *state = PortValue;
    return status;
}

int XNUCLEO53L3A2_ResetId(int DevNo, int state) {
    int status;
    switch( DevNo ){
    case XNUCLEO53L3A2_DEV_CENTER :
    case 'c' :
        CurIOVal.bytes[3]&=~0x80; /* bit 15 expender 1  => byte #3 */
        if( state )
            CurIOVal.bytes[3]|=0x80; /* bit 15 expender 1  => byte #3 */
        status= _ExpanderWR(I2cExpAddr1, GPSR+1, &CurIOVal.bytes[3], 1);
        break;
    case XNUCLEO53L3A2_DEV_LEFT :
    case 'l' :
        CurIOVal.bytes[1]&=~0x40; /* bit 14 expender 0 => byte #1*/
        if( state )
            CurIOVal.bytes[1]|=0x40; /* bit 14 expender 0 => byte #1*/
        status= _ExpanderWR(I2cExpAddr0, GPSR+1, &CurIOVal.bytes[1], 1);
        break;
    case 'r' :
    case XNUCLEO53L3A2_DEV_RIGHT :
        CurIOVal.bytes[1]&=~0x80; /* bit 15 expender 0  => byte #1 */
        if( state )
            CurIOVal.bytes[1]|=0x80; /* bit 15 expender 0 => byte #1*/
        status= _ExpanderWR(I2cExpAddr0, GPSR+1, &CurIOVal.bytes[1], 1);
        break;
    default:
        XNUCLEO53L3A2_ErrLog("Invalid DevNo %d",DevNo);
        status = -1;
        goto done;
    }
//error with valid id
    if( status ){
        XNUCLEO53L3A2_ErrLog("expander i/o error for DevNo %d state %d ",DevNo, state);
    }
done:
    return status;
}


/**
 * Set all i2c expended gpio in one go
 * @return i/o operation status
 */
static int _ExpandersSetAllIO(void){
    int status;
    status = _ExpanderWR(I2cExpAddr0, GPSR, &CurIOVal.bytes[0], 2);
    if( status ){
        goto done_err;
    }
    status = _ExpanderWR(I2cExpAddr1, GPSR, &CurIOVal.bytes[2], 2);
done_err:
    return status;
}

/**
 * STMPE1600  i2c Expender register read
 * @param I2cExpAddr Expender address
 * @param index      register index
 * @param data       read data buffer
 * @param n_data     number of byte to read
 * @return           of if ok else i2c I/O operation status
 */
static int _ExpanderRd(int I2cExpAddr, int index, uint8_t *data, int n_data) {

    int status;
    uint8_t RegAddr;
    RegAddr = index;
    XNUCLEO53L3A2_GetI2cBus();
    do {
        status = IIC_Write_nByte_t(I2cExpAddr, 1+1, &RegAddr);
        if (status)
            break;
        status = IIC_Read_nByte_t(I2cExpAddr, n_data, data);
    } while (0);
    XNUCLEO53L3A2_PutI2cBus();
    return status;
}

/**
 * STMPE1600 i2c Expender register write
 * @param I2cExpAddr Expender address
 * @param index      register index
 * @param data       data buffer
 * @param n_data     number of byte to write
 * @return           of if ok else i2c I/O operation status
 */
static int _ExpanderWR(int I2cExpAddr, int index, uint8_t *data, int n_data) {

    int status;
    uint8_t RegAddr[0x10];
    RegAddr[0] = index;
    memcpy(RegAddr + 1, data, n_data);
    XNUCLEO53L3A2_GetI2cBus();
    status = IIC_Write_nByte_t(I2cExpAddr, n_data+1, RegAddr);
    XNUCLEO53L3A2_PutI2cBus();
    return status;
}


/**
 * @defgroup XNUCLEO53L3A2_7Segment 7 segment display
 *
 * macro use for human readable segment building
 * @code
 *  --s0--
 *  s    s
 *  5    1
 *  --s6--
 *  s    s
 *  4    2
 *  --s3-- . s7 (dp)
 * @endcode
 *
 * @{
 */
/** decimal point bit mapping*  */
#define DP  (1<<7)

//VL6180 shield
//#define S0 (1<<0)
//#define S1 (1<<1)
//#define S2 (1<<2)
//#define S3 (1<<3)
//#define S4 (1<<4)
//#define S5 (1<<5)
//#define S6 (1<<6)

/** sgement s0 bit mapping*/
#define S0 (1<<3)
/** sgement s1 bit mapping*/
#define S1 (1<<5)
/** sgement s2 bit mapping*/
#define S2 (1<<6)
/** sgement s3 bit mapping*/
#define S3 (1<<4)
/** sgement s4 bit mapping*/
#define S4 (1<<0)
/** sgement s5 bit mapping*/
#define S5 (1<<1)
/** sgement s6 bit mapping*/
#define S6 (1<<2)

/**
 * build a character by defining the non lighted segment (not one and no DP)
 *
 * @param  ... literal sum and or combine of any macro to define any segment #S0 .. #S6
 *
 * example '9' is all segment on but S4
 * @code
 *   ['9']=           NOT_7_NO_DP(S4),
 * @endcode
 */
#define NOT_7_NO_DP( ... ) (uint8_t) ~( __VA_ARGS__ + DP )

/**
 * Ascii to 7 segment  lookup table
 *
 * Most common character are supported and follow http://www.twyman.org.uk/Fonts/
 * few extra special \@ ^~ ... etc are present for specific demo purpose
 */
static const uint8_t ascii_to_display_lut[256]={
      [' ']=           0,
      ['-']=           S6,
      ['_']=           S3,
      ['=']=           S3+S6,
      ['~']=           S0+S3+S6, /* 3 h bar */
      ['^']=           S0, /* use as top bar */

      ['?']=           NOT_7_NO_DP(S5+S3+S2),
      ['*']=           NOT_7_NO_DP(),
      ['[']=           S0+S3+S4+S5,
      [']']=           S0+S3+S2+S1,
      ['@']=           S0+S3,

      ['0']=           NOT_7_NO_DP(S6),
      ['1']=           S1+S2,
      ['2']=           S0+S1+S6+S4+S3,
      ['3']=           NOT_7_NO_DP(S4+S5),
      ['4']=           S5+S1+S6+S2,
      ['5']=           NOT_7_NO_DP(S1+S4),
      ['6']=           NOT_7_NO_DP(S1),
      ['7']=           S0+S1+S2,
      ['8']=           NOT_7_NO_DP(0),
      ['9']=           NOT_7_NO_DP(S4),

      ['a']=           S2+ S3+ S4+ S6 ,
      ['b']=           NOT_7_NO_DP(S0+S1),
      ['c']=           S6+S4+S3,
      ['d']=           NOT_7_NO_DP(S0+S5),
      ['e']=           NOT_7_NO_DP(S2),
      ['f']=           S6+S5+S4+S0, /* same as F */
      ['g']=           NOT_7_NO_DP(S4), /* same as 9 */
      ['h']=           S6+S5+S4+S2,
      ['i']=           S4,
      ['j']=           S1+S2+S3+S4,
      ['k']=           S6+S5+S4+S2, /* a h */
      ['l']=           S3+S4,
      ['m']=           S0+S4+S2, /* same as  */
      ['n']=           S2+S4+S6,
      ['o']=           S6+S4+S3+S2,
      ['p']=           NOT_7_NO_DP(S3+S2), // same as P
      ['q']=           S0+S1+S2+S5+S6,
      ['r']=           S4+S6,
      ['s']=           NOT_7_NO_DP(S1+S4),
      ['t']=           NOT_7_NO_DP(S0+S1+S2),
      ['u']=           S4+S3+S2+S5+S1, // U
      ['v']=           S4+S3+S2, // is u but u use U
      ['w']=           S1+S3+S5,
      ['x']=           NOT_7_NO_DP(S0+S3), // similar to H
      ['y']=           NOT_7_NO_DP(S0+S4),
      ['z']=           S0+S1+S6+S4+S3, // same as 2

      ['A']=           NOT_7_NO_DP(S3),
      ['B']=           NOT_7_NO_DP(S0+S1), /* as b  */
      ['C']=           S0+S3+S4+S5, // same as [
      ['E']=           NOT_7_NO_DP(S1+S2),
      ['F']=           S6+S5+S4+S0,
      ['G']=           NOT_7_NO_DP(S4), /* same as 9 */
      ['H']=           NOT_7_NO_DP(S0+S3),
      ['I']=           S1+S2,
      ['J']=           S1+S2+S3+S4,
      ['K']=           NOT_7_NO_DP(S0+S3), /* same as H */
      ['L']=           S3+S4+S5,
      ['M']=           S0+S4+S2, /* same as  m*/
      ['N']=           S2+S4+S6, /* same as n*/
      ['O']=           NOT_7_NO_DP(S6),
      ['P']=           NOT_7_NO_DP(S3+S2),
      ['Q']=           NOT_7_NO_DP(S3+S2),
      ['R']=           S4+S6,
      ['S']=           NOT_7_NO_DP(S1+S4), /* sasme as 5 */
      ['T']=           NOT_7_NO_DP(S0+S1+S2), /* sasme as t */
      ['U']=           NOT_7_NO_DP(S6+S0),
      ['V']=           S4+S3+S2, // is u but u use U
      ['W']=           S1+S3+S5,
      ['X']=           NOT_7_NO_DP(S0+S3), // similar to H
      ['Y']=           NOT_7_NO_DP(S0+S4),
      ['Z']=           S0+S1+S6+S4+S3, // same as 2
};

#undef S0
#undef S1
#undef S2
#undef S3
#undef S4
#undef S5
#undef S6
#undef DP

/** @} */

int XNUCLEO53L3A2_SetDisplayString(const char *str) {
    int status;
    uint32_t Segments;
    int BitPos;
    int i;

    for( i=0; i<4 && str[i]!=0; i++){
        Segments = (uint32_t)ascii_to_display_lut[(uint8_t)str[i]];
        Segments =(~Segments)&0x7F;
        BitPos=DisplayBitPos[i];
        CurIOVal.u32 &=~(0x7F<<BitPos);
        CurIOVal.u32 |= Segments<<BitPos;
    }
    /* clear unused digit */
    for( ; i<4;i++){
        BitPos=DisplayBitPos[i];
        CurIOVal.u32 |=0x7F<<BitPos;
    }
    status = _ExpandersSetAllIO();
    if( status ){
        BLE_RTT("Set i/o");
    }
    return status;
}

uint8_t get_tof_sensor_type(void)
{
    return tof_sensor_type;
}


int16_t get_vl53lx_distance(void)
{
    if(tof_sensor_type==1)
        return vl53lx_distance;
    else if(tof_sensor_type==0)
        return 0xffff;
}


void vl53lx_Init(void)
{
    uint32_t status = 0;
    uint8_t sensorState = 0;

    XNUCLEO53L3A2_Init();
    status = VL53LX_WaitDeviceBooted(Dev);

    status = VL53LX_DataInit(Dev);

    VL53LX_StartMeasurement(Dev);
    nrf_delay_ms(100);
    VL53LX_StopMeasurement(Dev);

    switch(StuHis.tof_mode)
    {
        case 1:
             status = VL53L1X_SensorInit(Dev);
            /* Optional functions to be used to change the main ranging parameters according the application requirements to get the best ranging performances */
            status = VL53L1X_SetDistanceMode(Dev, StuHis.tof_mode); /* 1=short, 2=long */
            VL53L1X_SetTimingBudgetInMs(Dev, 15); /* in ms possible values [20, 50, 100, 200, 500] */
            VL53L1X_SetInterMeasurementInMs(Dev, 17);

            break;
    }
    
    

    if(status){
        BLE_RTT("vl53lx_Init failed: error = %d \n", status);
        return;
    }
    else 
    {
        enable_pin_irq(GPIO1_PIN,true);
        tof_sensor_type = 1;
    }

    start_tof_measurement();
}

uint8_t start_tof_flg=0;

uint8_t get_tof_state(void)
{
    return start_tof_flg;
}


void set_vl53lx_config(uint8_t mode)
{
    uint32_t status;
    if(get_tof_state())
    {
        start_tof_flg=0;
        if(mode==1)
        {
            VL53L1X_StopRanging(Dev);
        }
        else 
        {
            VL53LX_StopMeasurement(Dev);
        }

    }

    if(mode==1)
    {
        VL53L1X_SensorInit(Dev);
        VL53L1X_SetDistanceMode(Dev, mode);
        VL53L1X_SetTimingBudgetInMs(Dev, 15); /* in ms possible values [20, 50, 100, 200, 500] */
        VL53L1X_SetInterMeasurementInMs(Dev, 17);

    }
    else
    {
        VL53LX_DataInit(Dev);

    }
    


}

void start_tof_measurement(void)
{
    uint32_t status;

    switch(StuHis.tof_mode)
    {
        case 1:
            status = VL53L1X_StartRanging(Dev);   /* This function has to be called to enable the ranging */
            VL53L1X_ClearInterrupt(Dev);
            break;

        case 2:
            status = VL53LX_StartMeasurement(Dev);
            break;
        
    }
    
    if(tof_sensor_type)
    {
        start_tof_flg=1;
    }
    

    if(status){
        BLE_RTT("VL53LX_StartMeasurement failed: error = %d \n",status);
        return;
    }

}


void tof_StopMeasurement(void)
{
    uint32_t status;

    if(start_tof_flg)
    {
        switch(StuHis.tof_mode)
        {
            case 1:
                VL53L1X_StopRanging(Dev);
                break;
            
            case 2:
                VL53LX_StopMeasurement(Dev);
                break;
        }
    }
    
}


void set_tof_int_flg(void)
{
    tof_int_flg = 1;
}

void calculate_vl53lx_distance(void)
{
    uint32_t status,measurement_status;
    uint16_t RangeData[4];

    uint8_t d_status;
    VL53LX_MultiRangingData_t MultiRangingData;
    VL53LX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
    uint8_t NewDataReady=0;
    int no_of_object_found=0,j;
    static uint8_t Stream_cnt = 0;

        
    if((tof_int_flg==0)
        ||(tof_sensor_type==0))
    {
        return;
    }
    tof_int_flg = 0;

    switch(StuHis.tof_mode)
    {
        case 1:
            {
                VL53L1X_GetRangeStatus(Dev, &d_status);
                VL53L1X_GetDistance(Dev, &RangeData[1]);

                if(d_status==0)
                {
                    BLE_RTT("status=%d, check distance=%5dmm",d_status,RangeData[1]);
                    vl53lx_distance = RangeData[1];

                }
                else
                {
                    BLE_RTT("status=%d, check distance fail",d_status);
                }
                BLE_RTT ("\n");

                VL53L1X_ClearInterrupt(Dev); 
                VL53L1X_StopRanging(Dev);
                start_tof_flg=0;

            }
            break;
        

        case 2:
            status = VL53LX_GetMeasurementDataReady(Dev, &NewDataReady);                        
            nrf_delay_ms(1); // 1 ms polling period, could be longer.
            if((!status)&&(NewDataReady!=0))
            {
                status = VL53LX_GetMultiRangingData(Dev, pMultiRangingData);
                no_of_object_found=pMultiRangingData->NumberOfObjectsFound;
                //BLE_RTT("Count=%5d, ", Stream_cnt);
                //BLE_RTT("#Objs=%1d ", no_of_object_found);
                Stream_cnt++;

            
                if(pMultiRangingData->RangeData[0].RangeStatus == 0)
                {
                    vl53lx_distance = pMultiRangingData->RangeData[0].RangeMilliMeter;  
                        BLE_RTT("status=%d, check distance=%5dmm",
                             pMultiRangingData->RangeData[0].RangeStatus,
                             pMultiRangingData->RangeData[0].RangeMilliMeter);
                }
                else
                {
                    BLE_RTT("status=%d, check distance fail",
                             pMultiRangingData->RangeData[0].RangeStatus);
                }
                BLE_RTT ("\n");
                if(Stream_cnt>=StuHis.tof_times)
                {
                    Stream_cnt=0;
                    status = VL53LX_StopMeasurement(Dev);
                    start_tof_flg = 0;
                    if(status)
                        BLE_RTT("VL53LX_StopMeasurement fail %x\r\n",status);
                    break;
                }  

                //if (pMultiRangingData->RangeData[0].RangeStatus==0xff)
                {   
                    status = VL53LX_ClearInterruptAndStartMeasurement(Dev);
                }
            }
            break;
    }

}




