/*
 Copyright (c) 2021, STMicroelectronics - All Rights Reserved

 This file : part of VL53L3CX ULP and : dual licensed,
 either 'STMicroelectronics
 Proprietary license'
 or 'BSD 3-clause "New" or "Revised" License' , at your option.

*******************************************************************************

 'STMicroelectronics Proprietary license'

*******************************************************************************

 License terms: STMicroelectronics Proprietary in accordance with licensing
 terms at www.st.com/sla0081

 STMicroelectronics confidential
 Reproduction and Communication of this document : strictly prohibited unless
 specifically authorized in writing by STMicroelectronics.


*******************************************************************************

 Alternatively, VL53L3CX ULP may be distributed under the terms of
 'BSD 3-clause "New" or "Revised" License', in which case the following
 provisions apply instead of the ones mentioned above :

*******************************************************************************

 License terms: BSD 3-clause "New" or "Revised" License.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 3. Neither the name of the copyright holder nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
*/

/**
 * @file  VL53L3CX_ULP_api.c
 * @brief Functions implementation
 */

#include "VL53L3CX_ULP_api.h"
#include "vl53lx_wait.h"
#include <math.h>

static const uint8_t VL53L3CX_ULP_DEFAULT_CONFIGURATION[] = {
	0x00, /* 0x2d */
	0x00, /* 0x2e */
	0x00, /* 0x2f */
	0x11, /* 0x30 */
	0x02, /* 0x31 */
	0x00, /* 0x32 */
	0x02, /* 0x33 */
	0x08, /* 0x34 */
	0x00, /* 0x35 */
	0x08, /* 0x36 */
	0x10, /* 0x37 */
	0x01, /* 0x38 */
	0x01, /* 0x39 */
	0x00, /* 0x3a */
	0x00, /* 0x3b */
	0x00, /* 0x3c */
	0x00, /* 0x3d */
	0xff, /* 0x3e */
	0x00, /* 0x3f */
	0x0F, /* 0x40 */
	0x00, /* 0x41 */
	0x00, /* 0x42 */
	0x00, /* 0x43 */
	0x00, /* 0x44 */
	0x00, /* 0x45 */
	0x20, /* 0x46 */
	0x0b, /* 0x47 */
	0x00, /* 0x48 */
	0x00, /* 0x49 */
	0x02, /* 0x4a */
	0x14, /* 0x4b */
	0x21, /* 0x4c */
	0x00, /* 0x4d */
	0x00, /* 0x4e */
	0x05, /* 0x4f */
	0x00, /* 0x50 */
	0x00, /* 0x51 */
	0x00, /* 0x52 */
	0x00, /* 0x53 */
	0xc8, /* 0x54 */
	0x00, /* 0x55 */
	0x00, /* 0x56 */
	0x38, /* 0x57 */
	0xff, /* 0x58 */
	0x01, /* 0x59 */
	0x00, /* 0x5a */
	0x08, /* 0x5b */
	0x00, /* 0x5c */
	0x00, /* 0x5d */
	0x00, /* 0x5e */
	0x01, /* 0x5f */
	0x07, /* 0x60 */
	0x00, /* 0x61 */
	0x02, /* 0x62 */
	0x05, /* 0x63 */
	0x00, /* 0x64 */
	0xb4, /* 0x65 */
	0x00, /* 0x66 */
	0xbb, /* 0x67 */
	0x08, /* 0x68 */
	0x38, /* 0x69 */
	0x00, /* 0x6a */
	0x00, /* 0x6b */
	0x00, /* 0x6c */
	0x00, /* 0x6d */
	0x0f, /* 0x6e */
	0x89, /* 0x6f */
	0x00, /* 0x70 */
	0x00, /* 0x71 */
	0x00, /* 0x72 */
	0x00, /* 0x73 */
	0x00, /* 0x74 */
	0x00, /* 0x75 */
	0x00, /* 0x76 */
	0x01, /* 0x77 */
	0x07, /* 0x78 */
	0x05, /* 0x79 */
	0x06, /* 0x7a */
	0x06, /* 0x7b */
	0x00, /* 0x7c */
	0x00, /* 0x7d */
	0x02, /* 0x7e */
	0xc7, /* 0x7f */
	0xff, /* 0x80 */
	0x9B, /* 0x81 */
	0x00, /* 0x82 */
	0x00, /* 0x83 */
	0x00, /* 0x84 */
	0x01, /* 0x85 */
	0x00, /* 0x86 */
	0x00  /* 0x87 */
};


uint8_t VL53L3CX_ULP_GetSensorId(
		VL53LX_DEV dev,
		uint16_t *p_id)
{
	uint8_t status = VL53L3CX_ULP_ERROR_NONE;

	status |= VL53LX_RdWord(dev,
			VL53L3CX_ULP_IDENTIFICATION__MODEL_ID, p_id);
	return status;
}

uint8_t VL53L3CX_ULP_SetI2CAddress(
		VL53LX_DEV dev,
		uint8_t new_address)
{
	uint8_t status = VL53L3CX_ULP_ERROR_NONE;

	status |= VL53LX_WrByte(dev, VL53L3CX_ULP_I2C_SLAVE__DEVICE_ADDRESS,
			(uint8_t)(new_address >> (uint8_t)1));
	return status;
}

uint8_t VL53L3CX_ULP_SensorInit(
		VL53LX_DEV dev)
{
	uint8_t status = VL53L3CX_ULP_ERROR_NONE;
	uint8_t Addr, tmp;
	uint8_t continue_loop = 1;
	uint16_t i = 0;

	do{
		status |= VL53LX_RdByte(dev,
				VL53L3CX_ULP_FIRMWARE__SYSTEM_STATUS, &tmp);

		if(tmp == (uint8_t)0x3) /* Sensor booted */
		{
			continue_loop = (uint8_t)0;
		}
		else if(i < (uint16_t)1000)       /* Wait for boot */
		{
			i++;
		}
		else /* Timeout 1000ms reached */
		{
			continue_loop = (uint8_t)0;
			status |= (uint8_t)VL53L3CX_ULP_ERROR_TIMEOUT;
		}
		VL53LX_WaitMs(dev,1);
	}while(continue_loop == (uint8_t)1);

	/* Load default configuration */
	for (Addr = (uint8_t)0x2D; Addr <= (uint8_t)0x87; Addr++)
	{
		status |= VL53LX_WrByte(dev, Addr,
				VL53L3CX_ULP_DEFAULT_CONFIGURATION[
                                  Addr - (uint8_t)0x2D]);
	}

	/* Start VHV */
	status |= VL53LX_WrByte(dev, VL53L3CX_ULP_SYSTEM_START, (uint8_t)0x40);
	i  = (uint8_t)0;
	continue_loop = (uint8_t)1;
	do{
		status |= VL53L3CX_ULP_CheckForDataReady(dev, &tmp);
		if(tmp == (uint8_t)1) /* Data ready */
		{
			continue_loop = (uint8_t)0;
		}
		else if(i < (uint16_t)1000)       /* Wait for answer */
		{
			i++;
		}
		else /* Timeout 1000ms reached */
		{
			continue_loop = (uint8_t)0;
			status |= (uint8_t)VL53L3CX_ULP_ERROR_TIMEOUT;
		}
		VL53LX_WaitMs(dev,1);
	}while(continue_loop == (uint8_t)1);

	status |= VL53L3CX_ULP_ClearInterrupt(dev);
	status |= VL53L3CX_ULP_StopRanging(dev);
	status |= VL53LX_WrByte(dev,
			VL53L3CX_ULP_VHV_CONFIG__TIMEOUT_MACROP_LOOP_BOUND,
                        (uint8_t)0x09);
	status |= VL53LX_WrByte(dev, 0x0B, (uint8_t)0);
	status |= VL53LX_WrWord(dev, 0x0024, 0x500);
	status |= VL53LX_WrByte(dev, 0x81, 0b10001010);
	status |= VL53LX_WrByte(dev, 0x004B, 0x03);

	/* Set default inter-measurement */
	status |= VL53L3CX_ULP_SetInterMeasurementInMs(dev, 1000);

	return status;
}

uint8_t VL53L3CX_ULP_CheckForDataReady(
		VL53LX_DEV dev,
		uint8_t *p_is_data_ready)
{
	uint8_t status = VL53L3CX_ULP_ERROR_NONE;
	uint8_t temp;
	uint8_t int_pol;

	status |= VL53LX_RdByte(dev, VL53L3CX_ULP_GPIO_HV_MUX__CTRL, &temp);
	temp = temp & (uint8_t)0x10;
	temp = temp >> 4;

	if (temp == (uint8_t)1)
	{
		int_pol = (uint8_t)0;
	}
	else
	{
		int_pol = (uint8_t)1;
	}

	status |= VL53LX_RdByte(dev, VL53L3CX_ULP_GPIO__TIO_HV_STATUS, &temp);

	if ((temp & (uint8_t)1) == int_pol)
	{
		*p_is_data_ready = (uint8_t)1;
	}
	else
	{
		*p_is_data_ready = (uint8_t)0;
	}

	return status;
}

uint8_t VL53L3CX_ULP_ClearInterrupt(
		VL53LX_DEV dev)
{
	uint8_t status = VL53L3CX_ULP_ERROR_NONE;

	status |= VL53LX_WrByte(dev, VL53L3CX_ULP_SYSTEM__INTERRUPT_CLEAR, 0x01);
	return status;
}

uint8_t VL53L3CX_ULP_StartRangingSingleShot(
		VL53LX_DEV dev)
{
	uint8_t status = VL53L3CX_ULP_ERROR_NONE;

	status |= VL53LX_WrByte(dev, VL53L3CX_ULP_SYSTEM_START, 0x10);
	return status;
}

uint8_t VL53L3CX_ULP_StartRanging(
		VL53LX_DEV dev)
{
	uint8_t status = VL53L3CX_ULP_ERROR_NONE;

	status |= VL53LX_WrByte(dev, VL53L3CX_ULP_SYSTEM_START, 0x40);
	return status;
}

uint8_t VL53L3CX_ULP_StopRanging(
		VL53LX_DEV dev)
{
	uint8_t status = VL53L3CX_ULP_ERROR_NONE;

	status |= VL53LX_WrByte(dev, VL53L3CX_ULP_SYSTEM_START, 0x00);
	return status;
}

uint8_t VL53L3CX_ULP_DumpDebugData(
		VL53LX_DEV dev,
		uint8_t *p_measurement_status,
		uint16_t *p_estimated_distance_mm,
		uint16_t *p_sigma_mm,
		uint16_t *p_signal_kcps,
		uint16_t *p_ambient_kcps)
{
	uint8_t status = VL53L3CX_ULP_ERROR_NONE;

	uint8_t status_rtn[24] = { 255, 255, 255, 5, 2, 4, 1, 7, 3, 0,
		255, 255, 9, 13, 255, 255, 255, 255, 10, 6,
		255, 255, 11, 12
	};

	status |= VL53LX_RdByte(dev, 0x0089, p_measurement_status);
	status |= VL53LX_RdWord(dev, 0x0096, p_estimated_distance_mm);
	status |= VL53LX_RdWord(dev, 0x008E, p_signal_kcps);
	status |= VL53LX_RdWord(dev, 0x0092, p_sigma_mm);
	status |= VL53LX_RdWord(dev, 0x0090, p_ambient_kcps);

	(*p_signal_kcps) *= (uint16_t)8;
	(*p_sigma_mm) /= (uint16_t)4;
	(*p_ambient_kcps) *= (uint16_t)8;

	*p_measurement_status = (*p_measurement_status) & (uint8_t)0x1F;
	if ((*p_measurement_status) < (uint8_t)24)
	{
		*p_measurement_status = status_rtn[(*p_measurement_status)];
	}

	return status;
}


uint8_t VL53L3CX_ULP_SetMacroTiming(
		VL53LX_DEV dev,
		uint16_t macro_timing)
{
	uint8_t status = VL53L3CX_ULP_ERROR_NONE;

	if(((uint16_t)macro_timing >= (uint16_t)1)
			&& ((uint16_t)macro_timing <= (uint16_t)255))
	{
		status |= VL53LX_WrWord(dev,
				VL53L3CX_ULP_RANGE_CONFIG_A, macro_timing);
		status |= VL53LX_WrWord(dev,
				VL53L3CX_ULP_RANGE_CONFIG_B,
                                macro_timing + (uint16_t)0x1);
	}
	else
	{
		status = VL53L3CX_ULP_ERROR_INVALID_ARGUMENT;
	}

	return status;
}

uint8_t VL53L3CX_ULP_GetMacroTiming(
		VL53LX_DEV dev,
		uint16_t *p_macro_timing)
{
	uint8_t status = VL53L3CX_ULP_ERROR_NONE;

	status |= VL53LX_RdWord(dev,
			VL53L3CX_ULP_RANGE_CONFIG_A, p_macro_timing);

	return status;
}

uint8_t VL53L3CX_ULP_SetInterMeasurementInMs(
		VL53LX_DEV dev,
		uint32_t inter_measurement_ms)
{
	uint8_t status = VL53L3CX_ULP_ERROR_NONE;
	uint16_t clock_pll;
        float_t inter_measurement_factor = (float_t)1.055;

	if(((uint32_t)inter_measurement_ms >= (uint32_t)10)
			&& ((uint32_t)inter_measurement_ms <= (uint32_t)60000))
	{
		status |= VL53LX_RdWord(dev,
				VL53L3CX_ULP_RESULT__OSC_CALIBRATE_VAL, &clock_pll);
		clock_pll = clock_pll & (uint16_t)0x3FF;
                inter_measurement_factor = inter_measurement_factor
                        * (float_t)inter_measurement_ms
                        * (float_t)clock_pll;

		status |= VL53LX_WrDWord(dev, VL53L3CX_ULP_INTERMEASUREMENT_MS,
				(uint32_t)inter_measurement_factor);
	}
	else
	{
		status = VL53L3CX_ULP_ERROR_INVALID_ARGUMENT;
	}

	return status;
}

uint8_t VL53L3CX_ULP_GetInterMeasurementInMs(
		VL53LX_DEV dev,
		uint32_t *p_inter_measurement_ms)
{
	uint8_t status = VL53L3CX_ULP_ERROR_NONE;
	uint16_t clock_pll;
	uint32_t tmp;
	float_t clock_pll_factor = (float_t)1.055;

	status |= VL53LX_RdDWord(dev, VL53L3CX_ULP_INTERMEASUREMENT_MS, &tmp);
	status |= VL53LX_RdWord(dev,
			VL53L3CX_ULP_RESULT__OSC_CALIBRATE_VAL, &clock_pll);

        if(clock_pll == (uint16_t)0)
        {
          status |= VL53L3CX_ULP_ERROR_INVALID_ARGUMENT;
        }
        else
        {
          	clock_pll = clock_pll & (uint16_t)0x3FF;
                clock_pll_factor = clock_pll_factor * (float_t)clock_pll;
                clock_pll = (uint16_t)clock_pll_factor;
                *p_inter_measurement_ms = (uint16_t)(tmp/(uint32_t)clock_pll);
        }

	return status;
}

uint8_t VL53L3CX_ULP_SetROI(
		VL53LX_DEV dev,
		uint8_t roi_width)
{
	uint8_t tmp, status = VL53L3CX_ULP_ERROR_NONE;

	status |= VL53LX_RdByte(dev, 0x013E, &tmp);

	if(((roi_width >= (uint8_t)4))
			&& ((roi_width <= (uint8_t)16)))
	{
		if (roi_width > (uint8_t)10)
		{
			tmp = (uint8_t)199;
		}
		status |= VL53LX_WrByte(dev, 0x007F, tmp);
		status |= VL53LX_WrByte(dev, 0x0080,
				((roi_width - (uint8_t)1) << 4)
                                  | (roi_width - (uint8_t)1));
	}
	else
	{
		status |= VL53L3CX_ULP_ERROR_INVALID_ARGUMENT;
	}

	return status;
}

uint8_t VL53L3CX_ULP_GetROI(
		VL53LX_DEV dev,
		uint8_t *p_roi_width)
{
	uint8_t tmp, status = VL53L3CX_ULP_ERROR_NONE;

	status |= VL53LX_RdByte(dev, 0x0080, &tmp);
	*p_roi_width = (tmp & (uint8_t)0x0F) + (uint8_t)1;

	return status;
}

uint8_t VL53L3CX_ULP_SetInterruptConfiguration(
		VL53LX_DEV dev,
		uint16_t distance_threshold_mm,
		uint8_t enable_interrupt_only_below_threshold)
{
	uint8_t tmp, status = VL53L3CX_ULP_ERROR_NONE;

	if(enable_interrupt_only_below_threshold == (uint8_t)0)
	{
		tmp = (uint8_t)0x20;
	}
	else
	{
		tmp = (uint8_t)0x0;
	}

	status |= VL53LX_WrByte(dev,
			VL53L3CX_ULP_SYSTEM__INTERRUPT, tmp);
	status |= VL53LX_WrWord(dev,
			VL53L3CX_ULP_THRESH_HIGH, distance_threshold_mm);
	status |= VL53LX_WrWord(dev,
			VL53L3CX_ULP_THRESH_LOW, distance_threshold_mm);
	return status;
}

uint8_t VL53L3CX_ULP_GetInterruptConfiguration(
		VL53LX_DEV dev,
		uint16_t *p_distance_threshold_mm,
		uint8_t *p_interrupt_enabled_only_below_threshold)
{
	uint8_t tmp, status = VL53L3CX_ULP_ERROR_NONE;

	status |= VL53LX_RdWord(dev,
			VL53L3CX_ULP_THRESH_HIGH, p_distance_threshold_mm);
	status |= VL53LX_RdByte(dev,
			VL53L3CX_ULP_SYSTEM__INTERRUPT, &tmp);

	if(tmp == (uint8_t)0x20)
	{
		*p_interrupt_enabled_only_below_threshold = 0;
	}
	else
	{
		*p_interrupt_enabled_only_below_threshold = 1;
	}

	return status;
}

uint8_t VL53L3CX_ULP_SetSignalThreshold(
		VL53LX_DEV dev,
		uint16_t signal_kcps)
{
	uint8_t status = VL53L3CX_ULP_ERROR_NONE;

	if(((signal_kcps >= (uint16_t)1))
			&& ((signal_kcps <= (uint16_t)16384)))
	{
		status |= VL53LX_WrWord(dev,
				VL53L3CX_ULP_MIN_COUNT_RATE_RTN_LIMIT_MCPS,signal_kcps>>3);
	}
	else
	{
		status |= VL53L3CX_ULP_ERROR_INVALID_ARGUMENT;
	}

	return status;
}

uint8_t VL53L3CX_ULP_GetSignalThreshold(
		VL53LX_DEV 	dev,
		uint16_t 	*p_signal_kcps)
{
	uint8_t status = VL53L3CX_ULP_ERROR_NONE;
	uint16_t tmp = 0;

	status |= VL53LX_RdWord(dev,
			VL53L3CX_ULP_MIN_COUNT_RATE_RTN_LIMIT_MCPS, &tmp);
	*p_signal_kcps = tmp <<3;

	return status;
}

uint8_t VL53L3CX_ULP_SetSigmaThreshold(
		VL53LX_DEV 	dev,
		uint16_t 	sigma_mm)
{
	uint8_t status = VL53L3CX_ULP_ERROR_NONE;

	if(sigma_mm>(uint16_t)((uint16_t)0xFFFF>>2))
	{
		status |= (uint8_t)VL53L3CX_ULP_ERROR_INVALID_ARGUMENT;
	}
	else
	{
		status |= VL53LX_WrWord(dev,
			VL53L3CX_ULP_RANGE_CONFIG__SIGMA_THRESH, sigma_mm<<2);
	}

	return status;
}

uint8_t VL53L3CX_ULP_GetSigmaThreshold(
		VL53LX_DEV 	 dev,
		uint16_t 	*p_sigma_mm)
{
	uint8_t status = VL53L3CX_ULP_ERROR_NONE;

	status |= VL53LX_RdWord(dev,
			VL53L3CX_ULP_RANGE_CONFIG__SIGMA_THRESH, p_sigma_mm);
	*p_sigma_mm = *p_sigma_mm >> 2;

	return status;
}
