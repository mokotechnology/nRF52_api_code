#include "lis3dh_driver.h"
#include "spi_io.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "bsp.h"



/******************************************/
#define SPI_BUFSIZE      8
/*****************************************/

/******************************************
* function : LIS3DH_ReadReg
* return   : SPISlaveBuffer ,and true
****************************************/
bool LIS3DH_ReadReg(uint8_t Reg, uint8_t* Data)// note!!! use this function for your project.
{
    uint8_t SPIMast_Buf[SPI_BUFSIZE];
    uint8_t SPISla_Buf[SPI_BUFSIZE];

    /* Write transaction */
//  uint8_t transfer_size = 2;
    SPIMast_Buf[0] = Reg | LIS3DH_READBIT;

    SPI_Enable_CS();
    software_spi00_send_dat(SPIMast_Buf,2);
    software_spi00_receiver_buf(SPISla_Buf,2);
    SPI_Disable_CS();

    /* Send received value back to the caller */
    *Data = SPISla_Buf[1];
//	BLE_RTT("Read-SPIAddr : %x, Data : %x\r\n",Reg,SPISla_Buf[1]);
    return true;
}

/*********************************
*function: lis3dh Ð´¼Ä´æÆ÷
* return : true
************************************/
bool LIS3DH_WriteReg(uint8_t WriteAddr, uint8_t Data)
{
    uint8_t SPIMast_Buf[SPI_BUFSIZE];

//  SPIReadLength = 0;
    SPIMast_Buf[0] = (WriteAddr&0x7f);
    SPIMast_Buf[1] = (Data);
//	BLE_RTT("Write-SPIAddr : %x, Data : %x\r\n",WriteAddr,Data);
    /* Check if we got an ACK or TIMEOUT error */
    SPI_Enable_CS();
    software_spi00_send_dat(SPIMast_Buf,2);
    SPI_Disable_CS();

    return true;
}




/*******************************************************************************
* Function Name  : LIS3DH_SetMode
* Description    : Sets LIS3DH Operating Mode
* Input          : Modality (LIS3DH_NORMAL, LIS3DH_LOW_POWER, LIS3DH_POWER_DOWN)
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
bool LIS3DH_SetMode(LIS3DH_Mode_t md)
{
    uint8_t value;
    uint8_t value2;
    static   uint8_t ODR_old_value;

    if(!LIS3DH_ReadReg(LIS3DH_CTRL_REG1, &value) )
        return MEMS_ERROR;

    if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG4, &value2) )
        return MEMS_ERROR;

    if((value & 0xF0)==0)
        value = value | (ODR_old_value & 0xF0); //if it comes from POWERDOWN

    switch(md)
    {
//        case LIS3DH_POWER_DOWN:
//            ODR_old_value = value;
//            value &= 0x0F;
//            break;

        case LIS3DH_HIGH:
            value &= 0xF7;
            value |=  (MEMS_RESET<<LIS3DH_LPEN);
            value2 &= 0xF7;
            value2 |= (MEMS_SET<<LIS3DH_HR); //reset HighResolution_BIT
            break;


        case LIS3DH_NORMAL:
            value &= 0xF7;
            value |= (MEMS_RESET<<LIS3DH_LPEN);
            value2 &= 0xF7;
            value2 |= (MEMS_RESET<<LIS3DH_HR);   //set HighResolution_BIT
            break;

        case LIS3DH_LOW_POWER:
            value &= 0xF7;
            value |=  (MEMS_SET<<LIS3DH_LPEN);
            value2 &= 0xF7;
            value2 |= (MEMS_RESET<<LIS3DH_HR); //reset HighResolution_BIT
            break;

        default:
            return MEMS_ERROR;
    }

    if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG1, value) )
        return MEMS_ERROR;

    if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG4, value2) )
        return MEMS_ERROR;
	BLE_RTT("LIS3DH_SetMode\r\n");
    return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetAxis
* Description    : Enable/Disable LIS3DH Axis
* Input          : LIS3DH_X_ENABLE/DISABLE | LIS3DH_Y_ENABLE/DISABLE | LIS3DH_Z_ENABLE/DISABLE
* Output         : None
* Note           : You MUST use all input variable in the argument, as example
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetAxis(uint8_t axis)
{
    uint8_t value;

    if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG1, &value) )
        return MEMS_ERROR;
    value &= 0xF8;
    value |= (0x07 & axis);
	
    if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG1, value) )
        return MEMS_ERROR;
	BLE_RTT("LIS3DH_SetAxis\r\n");
    return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetODR
* Description    : Sets LIS3DH Output Data Rate
* Input          : Output Data Rate
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetODR(LIS3DH_ODR_t ov)
{
    uint8_t value;

    if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG1, &value) )
        return MEMS_ERROR;

    value &= 0x0f;
    value |= ov<<LIS3DH_ODR_BIT;

    if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG1, value) )
        return MEMS_ERROR;

    return MEMS_SUCCESS;
}

/*******************************************************************************
* Function Name  : LIS3DH_SetFullScale
* Description    : Sets the LIS3DH FullScale
* Input          : LIS3DH_FULLSCALE_2/LIS3DH_FULLSCALE_4/LIS3DH_FULLSCALE_8/LIS3DH_FULLSCALE_16
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetFullScale(LIS3DH_Fullscale_t fs)
{
    uint8_t  value;

    if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG4, &value) )
        return MEMS_ERROR;

    value &= 0xCF;
    value |= (fs<<LIS3DH_FS);

    if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG4, value) )
        return MEMS_ERROR;

    return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetBLE
* Description    : Set Endianess (MSB/LSB)
* Input          : BLE_LSB / BLE_MSB
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetBLE(LIS3DH_Endianess_t ble)
{
    uint8_t value;

    if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG4, &value) )
        return MEMS_ERROR;

    value &= 0xBF;
    value |= (ble<<LIS3DH_BLE);

    if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG4, value) )
        return MEMS_ERROR;

    return MEMS_SUCCESS;
}

/*******************************************************************************
* Function Name  : LIS3DH_FIFOModeEnable
* Description    : Sets Fifo Modality
* Input          : LIS3DH_FIFO_DISABLE, LIS3DH_FIFO_BYPASS_MODE, LIS3DH_FIFO_MODE,
       LIS3DH_FIFO_STREAM_MODE, LIS3DH_FIFO_TRIGGER_MODE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_FIFOModeEnable(LIS3DH_FifoMode_t fm)
{
    uint8_t value;

    if(fm == LIS3DH_FIFO_DISABLE)
    {
        if( !LIS3DH_ReadReg(LIS3DH_FIFO_CTRL_REG, &value) )
            return MEMS_ERROR;

        value &= 0x1F;
        value |= (LIS3DH_FIFO_BYPASS_MODE<<LIS3DH_FM);

        if( !LIS3DH_WriteReg(LIS3DH_FIFO_CTRL_REG, value) )           //fifo mode bypass
            return MEMS_ERROR;
        if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG5, &value) )
            return MEMS_ERROR;

        value &= 0xBF;

        if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG5, value) )               //fifo disable
            return MEMS_ERROR;
    }

    if(fm == LIS3DH_FIFO_BYPASS_MODE)
    {
        if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG5, &value) )
            return MEMS_ERROR;

        value &= 0xBF;
        value |= MEMS_SET<<LIS3DH_FIFO_EN;

        if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG5, value) )               //fifo enable
            return MEMS_ERROR;
        if( !LIS3DH_ReadReg(LIS3DH_FIFO_CTRL_REG, &value) )
            return MEMS_ERROR;

        value &= 0x1f;
        value |= (fm<<LIS3DH_FM);                     //fifo mode configuration

        if( !LIS3DH_WriteReg(LIS3DH_FIFO_CTRL_REG, value) )
            return MEMS_ERROR;
    }

    if(fm == LIS3DH_FIFO_MODE)
    {
        if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG5, &value) )
            return MEMS_ERROR;

        value &= 0xBF;
        value |= MEMS_SET<<LIS3DH_FIFO_EN;

        if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG5, value) )               //fifo enable
            return MEMS_ERROR;
        if( !LIS3DH_ReadReg(LIS3DH_FIFO_CTRL_REG, &value) )
            return MEMS_ERROR;

        value &= 0x1f;
        value |= (fm<<LIS3DH_FM);                      //fifo mode configuration

        if( !LIS3DH_WriteReg(LIS3DH_FIFO_CTRL_REG, value) )
            return MEMS_ERROR;
    }

    if(fm == LIS3DH_FIFO_STREAM_MODE)
    {
        if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG5, &value) )
            return MEMS_ERROR;

        value &= 0xBF;
        value |= MEMS_SET<<LIS3DH_FIFO_EN;

        if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG5, value) )               //fifo enable
            return MEMS_ERROR;
        if( !LIS3DH_ReadReg(LIS3DH_FIFO_CTRL_REG, &value) )
            return MEMS_ERROR;

        value &= 0x1f;
        value |= (fm<<LIS3DH_FM);                      //fifo mode configuration

        if( !LIS3DH_WriteReg(LIS3DH_FIFO_CTRL_REG, value) )
            return MEMS_ERROR;
    }

    if(fm == LIS3DH_FIFO_TRIGGER_MODE)
    {
        if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG5, &value) )
            return MEMS_ERROR;

        value &= 0xBF;
        value |= MEMS_SET<<LIS3DH_FIFO_EN;

        if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG5, value) )               //fifo enable
            return MEMS_ERROR;
        if( !LIS3DH_ReadReg(LIS3DH_FIFO_CTRL_REG, &value) )
            return MEMS_ERROR;

        value &= 0x1f;
        value |= (fm<<LIS3DH_FM);                      //fifo mode configuration

        if( !LIS3DH_WriteReg(LIS3DH_FIFO_CTRL_REG, value) )
            return MEMS_ERROR;
    }

    return MEMS_SUCCESS;
}

/*******************************************************************************
* Function Name  : LIS3DH_GetAccAxesRaw
* Description    : Read the Acceleration Values Output Registers
* Input          : buffer to empity by AxesRaw_t Typedef
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_GetAccAxesRaw(AxesRaw_t* buff)
{
    int16_t value;
    uint8_t *valueL = ((uint8_t *)(&value)+1);
    uint8_t *valueH = (uint8_t *)(&value);

    if( !LIS3DH_ReadReg(LIS3DH_OUT_X_L, valueL) )
        return MEMS_ERROR;

    if( !LIS3DH_ReadReg(LIS3DH_OUT_X_H, valueH) )
        return MEMS_ERROR;

    buff->AXIS_X = value;

    if( !LIS3DH_ReadReg(LIS3DH_OUT_Y_L, valueL) )
        return MEMS_ERROR;

    if( !LIS3DH_ReadReg(LIS3DH_OUT_Y_H, valueH) )
        return MEMS_ERROR;

    buff->AXIS_Y = value;

    if( !LIS3DH_ReadReg(LIS3DH_OUT_Z_L, valueL) )
        return MEMS_ERROR;

    if( !LIS3DH_ReadReg(LIS3DH_OUT_Z_H, valueH) )
        return MEMS_ERROR;

    buff->AXIS_Z = value;

    return MEMS_SUCCESS;


    /*
    if( !LIS3DH_ReadReg(LIS3DH_OUT_X_L, valueL) )
      return MEMS_ERROR;

    if( !LIS3DH_ReadReg(LIS3DH_OUT_X_H, valueH) )
      return MEMS_ERROR;

    buff->AXIS_X = value;

    if( !LIS3DH_ReadReg(LIS3DH_OUT_Y_L, valueL) )
      return MEMS_ERROR;

    if( !LIS3DH_ReadReg(LIS3DH_OUT_Y_H, valueH) )
      return MEMS_ERROR;

    buff->AXIS_Y = value;

    if( !LIS3DH_ReadReg(LIS3DH_OUT_Z_L, valueL) )
      return MEMS_ERROR;

    if( !LIS3DH_ReadReg(LIS3DH_OUT_Z_H, valueH) )
      return MEMS_ERROR;

    buff->AXIS_Z = value;
    */

}

/*******************************************************************************
* Function Name  : LIS3DH_SetWaterMark
* Description    : Sets Watermark Value
* Input          : Watermark = [0,31]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetWaterMark(uint8_t wtm)
{
    uint8_t value;

    if(wtm > 31)
        return MEMS_ERROR;

    if( !LIS3DH_ReadReg(LIS3DH_FIFO_CTRL_REG, &value) )
        return MEMS_ERROR;

    value &= 0xE0;
    value |= wtm;

    if( !LIS3DH_WriteReg(LIS3DH_FIFO_CTRL_REG, value) )
        return MEMS_ERROR;

    return MEMS_SUCCESS;
}
/*******************************************************************************
* Function Name  : LIS3DH_SetBDU
* Description    : Enable/Disable Block Data Update Functionality
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetBDU(State_t bdu)
{
    uint8_t value;

    if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG4, &value) )
        return MEMS_ERROR;

    value &= 0x7F;
    value |= (bdu<<LIS3DH_BDU);

    if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG4, value) )
        return MEMS_ERROR;

    return MEMS_SUCCESS;
}

/*******************************************************************************
* Function Name  : LIS3DH_SetInt1Pin
* Description    : Set Interrupt1 pin Function
* Input          :  LIS3DH_CLICK_ON_PIN_INT1_ENABLE/DISABLE    | LIS3DH_I1_INT1_ON_PIN_INT1_ENABLE/DISABLE |              
                    LIS3DH_I1_INT2_ON_PIN_INT1_ENABLE/DISABLE  | LIS3DH_I1_DRDY1_ON_INT1_ENABLE/DISABLE    |              
                    LIS3DH_I1_DRDY2_ON_INT1_ENABLE/DISABLE     | LIS3DH_WTM_ON_INT1_ENABLE/DISABLE         |           
                    LIS3DH_INT1_OVERRUN_ENABLE/DISABLE  
* example        : SetInt1Pin(LIS3DH_CLICK_ON_PIN_INT1_ENABLE | LIS3DH_I1_INT1_ON_PIN_INT1_ENABLE |              
                    LIS3DH_I1_INT2_ON_PIN_INT1_DISABLE | LIS3DH_I1_DRDY1_ON_INT1_ENABLE | LIS3DH_I1_DRDY2_ON_INT1_ENABLE |
                    LIS3DH_WTM_ON_INT1_DISABLE | LIS3DH_INT1_OVERRUN_DISABLE   ) 
* Note           : To enable Interrupt signals on INT1 Pad (You MUST use all input variable in the argument, as example)
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetInt1Pin(uint8_t pinConf)
{
    uint8_t value;

    if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG3, &value) )
        return MEMS_ERROR;

    value &= 0x00;
    value |= pinConf;
    if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG3, value) )
        return MEMS_ERROR;

    return MEMS_SUCCESS;
}

/*******************************************************************************
* Function Name  : LIS3DH_SetFilterDataSel
* Description    : Set Filter Data Selection bypassed or sent to FIFO OUT register
* Input          : MEMS_SET, MEMS_RESET
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetFilterDataSel(State_t state)
{
    uint8_t value;

    if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG2, &value) )
        return MEMS_ERROR;

    value &= 0xF7;
    value |= (state<<LIS3DH_FDS);
    if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG2, value) )
        return MEMS_ERROR;

    return MEMS_SUCCESS;

}

/*******************************************************************************
* Function Name  : LIS3DH_HPFAOI1
* Description    : Enable/Disable High Pass Filter for AOI on INT_1
* Input          : MEMS_ENABLE/MEMS_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_HPFAOI1Enable(State_t hpfe)
{
    uint8_t value;

    if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG2, &value) )
        return MEMS_ERROR;

    value &= 0xFE;
    value |= (hpfe<<LIS3DH_HPIS1);
    if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG2, value) )
        return MEMS_ERROR;

    return MEMS_SUCCESS;
}
/*******************************************************************************
* Function Name  : LIS3DH_Int1LatchEnable
* Description    : Enable Interrupt 1 Latching function
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_Int1LatchEnable(State_t latch)
{
    uint8_t value;

    if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG5, &value) )
        return MEMS_ERROR;

    value &= 0xF7;
    value |= latch<<LIS3DH_LIR_INT1;
    if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG5, value) )
        return MEMS_ERROR;

    return MEMS_SUCCESS;
}
