#ifndef _STK832X_DRIVER_H
#define _STK832X_DRIVER_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>


#define STK_SPI_MODE	1
#define STK_8328_C		1
#define STK_8325		0

//range
#define STK832x_RANGE_2G		0x03
#define STK832x_RANGE_4G		0x05
#define STK832x_RANGE_8G		0x08

#define STK_INT1_ANY_MOT_EN		0x04

//ODR and power mode
#define	STK_SUSPEND_MODE		0x80	//idle mode
#define STK832x_SET_ODR_1HZ		0x7F	//low-power mode Duration (1Hz)
#define	STK832x_SET_ODR_10HZ	0x7A	//low-power mode Duration (10Hz)
#define	STK832x_SET_ODR_25HZ	0x76	//low-power mode Duration (25Hz)
#define	STK832x_SET_ODR_50HZ	0x74	//low-power mode Duration (50Hz)
#define	STK832x_SET_ODR_100HZ	0x72	//low-power mode Duration (100Hz)

//CHIP ID
#define STK8327_CHIPID_VAL	0x26	/* STK8327 CHIP ID = 0x26 */
#define STK8329_CHIPID_VAL	0x25	/* STK8329&STK8325 CHIP ID = 0x25 */
#define STK832x_CHIPID_VAL	0x23    /* STK8321 or STK8323 CHIP ID = 0x23 */

//SPI MODE
#define STK_SPI_3WM_EN		0x05
#define STK_SPI_3WM_DIS		0x04

//BANDWIDTH
#define STK_BW_1KHZ			0x0F
#define STK_BW_500HZ		0x0E
#define STK_BW_250HZ		0x0D
#define STK_BW_125HZ		0x0C

//ESM
#define STK_ESM_MODE_EN		0x08
#define STK_ESM_MODE_DIS	0x00

//INT CFG
#define	STK_SLP_EN_X			0x01
#define	STK_SLP_EN_Y			0x02
#define	STK_SLP_EN_Z			0x04
#define	STK_INT1_EN				0x04
#define STK_INT1_ACT_LEVEl_L	0x00
#define STK_INT1_ACT_LEVEl_H	0x01
#define STK_INT1_PUSH_PULL		0x00
#define STK_INT1_OPEN_DRAIN		0x02

//FIFO
#define FIFO_FRAME_MAX_CNT			32
#define FIFO_CNT_MASK				0x7f;
#define	FIFO_BYPASS_MODE			0x00
#define	FIFO_NORMAL_MODE			0x20
#define	FIFO_STREAM_TO_FIFO_MODE	0x60
#define	FIFO_BYPASS_TO_STREAM_MODE	0x80
#define	FIFO_STREAM_MODE			0xC0	


#define SPI_BUFSIZE      200

typedef enum
{
	STK_REG_CHIPID			=	0x00,
	STK_REG_XOUTL			=	0x02,
	STK_REG_XOUTH			=	0x03,
	STK_REG_YOUTL			=	0x04,
	STK_REG_YOUTH			=	0x05,
	STK_REG_ZOUTL			=	0x06,
	STK_REG_ZOUTH			=	0x07,
	STK_REG_FIFOSTS			=	0x0C,
	STK_REG_RANGESEL		=	0x0F,
	STK_REG_BWSEL			=	0x10,
	STK_REG_POWMODE			=	0x11,
	STK_REG_ODRMODE			=	0x12,
	STK_REG_DATASETUP		=	0x13,
	STK_REG_SWRST			=	0x14,
	STK_REG_INTEN1			=	0x16,
	STK_REG_INTMAP1			=	0x19,
	STK_REG_INTCFG1			=	0x20,
	STK_REG_SLOPEDLY		=	0x27,
	STK_REG_SLOPETHD		=	0x28,
	STK_REG_SIGMOT2			= 	0x2A,
	STK_REG_PRIMIF			=	0x33,
	STK_REG_INTFCFG			=	0x34,
	STK_REG_LPFCFG			=	0x35,
	STK_REG_FIFOWM			=	0x3D,
	STK_REG_FIFOMODE		=	0x3E,
	STK_REG_FIFODATA		=	0x3F,
	STK_REG_ENG_MODE		=	0x5E,
	
}STK_REG;

typedef struct
{
    int16_t AXIS_X;
    int16_t AXIS_Y;
    int16_t AXIS_Z;
}
STK_AxesRaw_t;




void STK832x_set_int_ths_valu(uint8_t set_valu);

bool STK832x_Check_chip_id(void);
void STK832x_Suspend_mode(bool en,uint8_t ODR);
int STK832x_Initialization(void);
uint8_t STK832x_Read_fifo_len(void);

void STK832x_Getfifo_buf(uint8_t frameSize,STK_AxesRaw_t* axes);
void read_xyz(void);
void set_mode(void);
void get_a_sensor_data(STK_AxesRaw_t* sensor_data);



void raw_change_To_mg(uint8_t frameSize,STK_AxesRaw_t *axes);
bool get_is_have_stk832x_sta(void);
#endif
