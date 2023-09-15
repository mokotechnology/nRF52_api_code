/*
* Copyright (c) 2021, STMicroelectronics - All Rights Reserved
*
* This file : part of VL53L3CX ULP and : dual licensed,
* either 'STMicroelectronics
* Proprietary license'
* or 'BSD 3-clause "New" or "Revised" License' , at your option.
*
********************************************************************************
*
* 'STMicroelectronics Proprietary license'
*
********************************************************************************
*
* License terms: STMicroelectronics Proprietary in accordance with licensing
* terms at www.st.com/sla0081
*
* STMicroelectronics confidential
* Reproduction and Communication of this document : strictly prohibited unless
* specifically authorized in writing by STMicroelectronics.
*
*
********************************************************************************
*
* Alternatively, VL53L3CX ULP may be distributed under the terms of
* 'BSD 3-clause "New" or "Revised" License', in which case the following
* provisions apply instead of the ones mentioned above :
*
********************************************************************************
*
* License terms: BSD 3-clause "New" or "Revised" License.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*
********************************************************************************
*
*/

/**
 * @file  VL53L3CX api.h
 * @brief Functions definition
 */

#ifndef VL53L3CX_ULP_API_H_
#define VL53L3CX_ULP_API_H_

#include "vl53lx_platform.h"

#define VL53L3CX_ULP_VERSION					"1.0.0"

/**
 * @brief Error types.
 */

#define VL53L3CX_ULP_ERROR_NONE				((uint8_t)0U)
#define VL53L3CX_ULP_ERROR_INVALID_ARGUMENT		((uint8_t)254U)
#define VL53L3CX_ULP_ERROR_TIMEOUT			((uint8_t)255U)


/**
 * @brief Firmware addresses.
 */


#define VL53L3CX_ULP_I2C_SLAVE__DEVICE_ADDRESS      ((uint16_t)0x0001)
#define VL53L3CX_ULP_VHV_CONFIG__TIMEOUT_MACROP_LOOP_BOUND  ((uint16_t)0x0008)
#define VL53L3CX_ULP_GPIO_HV_MUX__CTRL      ((uint16_t)0x0030)
#define VL53L3CX_ULP_GPIO__TIO_HV_STATUS    ((uint16_t)0x0031)
#define VL53L3CX_ULP_SYSTEM__INTERRUPT  ((uint16_t)0x0046)
#define VL53L3CX_ULP_RANGE_CONFIG_A     ((uint16_t)0x005E)
#define VL53L3CX_ULP_RANGE_CONFIG_B      ((uint16_t)0x0061)
#define VL53L3CX_ULP_RANGE_CONFIG__SIGMA_THRESH     ((uint16_t)0x0064)
#define VL53L3CX_ULP_MIN_COUNT_RATE_RTN_LIMIT_MCPS    ((uint16_t)0x0066)
#define VL53L3CX_ULP_INTERMEASUREMENT_MS ((uint16_t)0x006C)
#define VL53L3CX_ULP_THRESH_HIGH    ((uint16_t)0x0072)
#define VL53L3CX_ULP_THRESH_LOW     ((uint16_t)0x0074)
#define VL53L3CX_ULP_POWER_GO1     ((uint16_t)0x0083)
#define VL53L3CX_ULP_FIRMWARE_ENABLE     ((uint16_t)0x0085)
#define VL53L3CX_ULP_SYSTEM__INTERRUPT_CLEAR        ((uint16_t)0x0086)
#define VL53L3CX_ULP_SYSTEM_START     ((uint16_t)0x0087)
#define VL53L3CX_ULP_RESULT__RANGE_STATUS   ((uint16_t)0x0089)
#define VL53L3CX_ULP_RESULT__SPAD_NB   ((uint16_t)0x008C)
#define VL53L3CX_ULP_RESULT__SIGNAL_RATE   ((uint16_t)0x008E)
#define VL53L3CX_ULP_RESULT__AMBIENT_RATE   ((uint16_t)0x0090)
#define VL53L3CX_ULP_RESULT__SIGMA   ((uint16_t)0x0092)
#define VL53L3CX_ULP_RESULT__DISTANCE   ((uint16_t)0x0096)
#define VL53L3CX_ULP_RESULT__OSC_CALIBRATE_VAL      ((uint16_t)0x00DE)
#define VL53L3CX_ULP_FIRMWARE__SYSTEM_STATUS        ((uint16_t)0x00E5)
#define VL53L3CX_ULP_IDENTIFICATION__MODEL_ID       ((uint16_t)0x010F)

/**
 * @brief Functions definitions.
 */

/**
 * @brief This function is used to get the sensor id of VL53L3CX. The sensor id
 * should be 0xEAAA.
 * @param (uint16_t) dev : Device address.
 * @param (uint16_t) *p_id : Sensor id.
 * @return (uint8_t) status : 0 if OK.
 */

uint8_t VL53L3CX_ULP_GetSensorId(
		VL53LX_DEV dev,
		uint16_t *p_id);

/**
 * @brief This function sets a new I2C address to a sensor. It can be used for
 * example when multiple sensors share the same I2C bus.
 * @param (uint16_t) dev : Device to update.
 * @param (uint8_t) new_address : New I2C address.
 * @return (uint8_t) status : 0 if I2C address has been correctly
 * programmed.
 */

uint8_t VL53L3CX_ULP_SetI2CAddress(
		VL53LX_DEV dev,
		uint8_t new_address);

/**
 * @brief This function is used to initialize the sensor.
 * @param (uint16_t) dev : Device address.
 * @return (uint8_t) status : 0 if init is OK.
 */

uint8_t VL53L3CX_ULP_SensorInit(
		VL53LX_DEV dev);

/**
 * @brief This function check if a new data is available by polling a dedicated
 * register.
 * @param (uint16_t) dev : Device address.
 * @param (uint8_t) *p_is_data_ready : Pointer containing a flag to know if a
 * data is ready : 0 = no data ready, 1 = data ready.
 * @return (uint8_t) status : 0 if OK.
 */

uint8_t VL53L3CX_ULP_CheckForDataReady(
		VL53LX_DEV dev,
		uint8_t *p_is_data_ready);

/**
 * @brief This function clears the interrupt. It needs to be called after a
 * ranging data reading to arm the interrupt for the next data ready event.
 * @param (uint16_t) dev : Device address.
 * @return (uint8_t) status : 0 if OK.
 */

uint8_t VL53L3CX_ULP_ClearInterrupt(
		VL53LX_DEV dev);

/**
 * @brief This function starts a ranging session for only one measurement.
 * @param (uint16_t) dev : Device address.
 * @return (uint8_t) status : 0 if OK.
 */

uint8_t VL53L3CX_ULP_StartRangingSingleShot(
		VL53LX_DEV dev);

/**
 * @brief This function starts a ranging session. A manual clear interrupt has
 * to be done to restart the next measurement.
 * @param (uint16_t) dev : Device address.
 * @return (uint8_t) status : 0 if OK.
 */

uint8_t VL53L3CX_ULP_StartRanging(
		VL53LX_DEV dev);

/**
 * @brief This function stops the ranging in progress.
 * @param (uint16_t) dev : Device address.
 * @return (uint8_t) status : 0 if OK.
 */

uint8_t VL53L3CX_ULP_StopRanging(
		VL53LX_DEV dev);

/**
 * @brief This function can be used to dump the debug data (estimated distance,
 * measurement status, etc). It is particularly useful to tune the device for a
 * specific application. The function can be used when a new measurement is
 * ready, after clearing the interrupt.
 * @param (uint16_t) dev : Device address.
 * @param (uint8_t) *p_measurement_status : Measurement status. A value equal to
 * 0 means that the estimated distance should be close to the reality.
 * @param (uint16_t) *p_estimated_distance_mm : Estimated target distance in mm.
 * @param (uint8_t) *p_sigma_mm : Estimated measurements std deviation in mm.
 * @param (uint16_t) *p_signal_kcps : Estimated signal rate of the target in kcps.
 * @param (uint16_t) *p_signal_mm : Ambient noise from external light in kcps.
 * @return (uint8_t) status : 0 if OK.
 */

uint8_t VL53L3CX_ULP_DumpDebugData(
		VL53LX_DEV dev,
		uint8_t *p_measurement_status,
		uint16_t *p_estimated_distance_mm,
		uint16_t *p_sigma_mm,
		uint16_t *p_signal_kcps,
		uint16_t *p_ambient_kcps);

/**
 * @brief Macro timing is equivalent to integration time. A high value increases
 * the maximum ranging distance and accuracy, but it also increase the power
 * consumption. This function is used to program a new value.
 * @param (uint16_t) dev : Device address.
 * @param (uint16_t) macro_timing : Macro timing value. Value needs to be
 * between 1 and 255. Default is 1 (lowest power consumption).
 * @return (uint8_t) status : 0 if OK, or 254 if macro timing is invalid.
 */

uint8_t VL53L3CX_ULP_SetMacroTiming(
		VL53LX_DEV dev,
		uint16_t macro_timing);

/**
 * @brief Macro timing is equivalent to integration time. A high value increases
 * the maximum ranging distance and accuracy, but it also increase the power
 * consumption. This function is used to get the programmed value.
 * @param (uint16_t) dev : Device address.
 * @param (uint16_t) *p_macro_timing : Programmed macro timing value.
 * @return (uint8_t) status : 0 if OK.
 */

uint8_t VL53L3CX_ULP_GetMacroTiming(
		VL53LX_DEV dev,
		uint16_t *p_macro_timing);

/**
 * @brief This function can be used to program the time between 2
 * consecutive measurements.
 * @param (uint16_t) dev : Device address.
 * @param (uint32_t) inter_measurement_ms : Time in milliseconds between 2
 * measurements. Min value is 20ms and max is 60000ms. Default is 100ms.
 * @return (uint8_t) status : 0 if OK, or 254 if inter-measurement is invalid.
 */

uint8_t VL53L3CX_ULP_SetInterMeasurementInMs(
		VL53LX_DEV dev,
		uint32_t inter_measurement_ms);

/**
 * @brief This function can be used to get the programmed time between 2
 * consecutive measurements.
 * @param (uint16_t) dev : Device address.
 * @param (uint32_t) *p_inter_measurement_ms : Time in milliseconds between 2
 * measurements.
 * @return (uint8_t) status : 0 if OK.
 */

uint8_t VL53L3CX_ULP_GetInterMeasurementInMs(
		VL53LX_DEV dev,
		uint32_t *p_inter_measurement_ms);

/**
 * @brief This function can be used to change the number of SPADs enabled. The
 * VL53L3CX uses by default a matrix of 256 SPADs which can be reduced. A
 * reduced number of SPADs reduces the power consumption, but is also reduces
 * the maximum ranging distance and accuracy. By default all SPADs are enabled
 * (max ranging distance).
 * IMPORTANT : Reducing the number of SPADs does't reduce the FOV.
 * @param (uint16_t) dev : Device address.
 * @param (uint8_t) roi_width : Size of ROI. The minimum width is 4 (4x4=16
 * SPADs), and the maximum is 16 (16x16=256 SPADs).
 * @return (uint8_t) status : 0 if OK, or 254 if roi is invalid.
 */

uint8_t VL53L3CX_ULP_SetROI(
		VL53LX_DEV dev,
		uint8_t roi_width);

/**
 * @brief This function can be used to get the number of SPADs enabled. The
 * VL53L3CX uses by default a matrix of 256 SPADs which can be reduced. A
 * reduced number of SPADs reduces the power consumption, but is also reduces
 * the maximum ranging distance and accuracy. By default all SPADs are enabled
 * (max ranging distance).
 * IMPORTANT : Reducing the number of SPADs does't reduce the FOV.
 * @param (uint16_t) dev : Device address.
 * @param (uint8_t) *p_roi_width : Size of ROI.
 * @return (uint8_t) status : 0 if OK.
 */

uint8_t VL53L3CX_ULP_GetROI(
		VL53LX_DEV dev,
		uint8_t *p_roi_width);

/**
 * @brief This function can be used to program the interrupt. There are 2
 * possible configurations :
 *   - 1) Interrupt raised when a new measurement is ready, even if the distance
 *      is wrong. It can be used to debug and tune the sensor.
 *   - 2) Interrupt raised only when a target is detected. This is more for real
 *     scene applications.
 * By default the sensor is programmed for the first case, it raises an
 * interrupt any new measurement is ready.
 * @param (uint16_t) dev : Device address.
 * @param (uint16_t) distance_threshold_mm : Below this threshold, the
 * interrupt will be raised if the measurement is valid (status 0).
 * @param (uint8_t) enable_interrupt_only_below_threshold : If this flag is set
 * to 0, the sensor is programmed with the first configuration (raise an
 * interrupt when a new measurement is ready). Else, the sensor is programmed
 * to raise an interrupt ONLY when a valid distance below the threshold is
 * reported.
 * @return (uint8_t) status : 0 if OK.
 */

uint8_t VL53L3CX_ULP_SetInterruptConfiguration(
		VL53LX_DEV dev,
		uint16_t distance_threshold_mm,
		uint8_t enable_interrupt_only_below_threshold);

/**
 * @brief This function can be used to get the programmed the interrupt. There
 * are 2 possible configurations :
 *   - 1) Interrupt raised when a new measurement is ready, even if the distance
 *      is wrong. It can be used to debug and tune the sensor.
 *   - 2) Interrupt raised only when a target is detected. This is more for real
 *     scene applications.
 * By default the sensor is programmed for the first case, it raises an
 * interrupt any new measurement is ready.
 * @param (uint16_t) dev : Device address.
 * @param (uint16_t) *p_distance_threshold_mm : Below this threshold, the
 * interrupt will be raised if the measurement is valid (status 0).
 * @param (uint8_t) *p_interrupt_enabled_only_below_threshold : If this flag is
 * set to 0, the sensor is programmed with the first configuration (raise an
 * interrupt when a new measurement is ready). Else, the sensor is programmed
 * to raise an interrupt ONLY when a valid distance below the threshold is
 * reported.
 * @return (uint8_t) status : 0 if OK.
 */

uint8_t VL53L3CX_ULP_GetInterruptConfiguration(
		VL53LX_DEV dev,
		uint16_t *p_distance_threshold_mm,
		uint8_t *p_interrupt_enabled_only_below_threshold);

/**
 * @brief This function sets a new signal threshold in kcps. If a
 * target has a lower signal as the programmed value, the measurement status in
 * debug value will be equal to 2.
 * @param (uint16_t) dev : Device address.
 * @param (uint16_t) signal_kcps : New signal threshold in kcps. The default
 * value is 1500 kcps. Minimum is 1 kcps, and maximum is 16384 kcps.
 * @return (uint8_t) status : 0 if OK, or 254 if the threshold is not valid.
 */

uint8_t VL53L3CX_ULP_SetSignalThreshold(
		VL53LX_DEV dev,
		uint16_t signal_kcps);

/**
 * @brief This function returns the current signal threshold in kcps. If a
 * target has a lower signal as the programmed value, the measurement status in
 * debug value will be equal to 2.
 * @param (uint16_t) dev : Device address.
 * @param (uint16_t) *p_signal_kcps : Pointer of signal threshold in kcps.
 * @return (uint8_t) status : 0 if OK.
 */

uint8_t VL53L3CX_ULP_GetSignalThreshold(
		VL53LX_DEV 	dev,
		uint16_t 	*p_signal_kcps);

/**
 * @brief This function programs a new sigma threshold. The sigma corresponds to
 * the standard deviation of the returned pulse. If the computed sigma is above
 * the programmed value, the measurement status in debug value will be equal to
 * 1.
 * @param (uint16_t) dev : Device address.
 * @param (uint16_t) sigma_mm : New sigma threshold in mm. The default value is
 * 45mm. Minimum is 1mm, and maximum is 16383mm.
* @return (uint8_t) status : 0 if OK, or 254 if the threshold is not valid.
 */

uint8_t VL53L3CX_ULP_SetSigmaThreshold(
		VL53LX_DEV 	dev,
		uint16_t 	sigma_mm);

/**
 * @brief This function gets the current sigma threshold. The sigma corresponds
 * to the standard deviation of the returned pulse. If the computed sigma is
 * above the programmed value, the measurement status in debug value will be
 * equal to 1.
 * @param (uint16_t) dev : Device address.
 * @param (uint16_t) *p_sigma_mm : Current sigma threshold in mm.
 * @return (uint8_t) status : 0 if OK.
 */

uint8_t VL53L3CX_ULP_GetSigmaThreshold(
		VL53LX_DEV 	 dev,
		uint16_t 	*p_sigma_mm);

#endif
