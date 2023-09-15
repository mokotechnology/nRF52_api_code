/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */


#include "iic_io.h"
#include "utility.h"


#define  TWI_DELAY_HRS() delay_us(1)
#define IIC_DELAY(A)    delay_us((A)*100)

uint8_t sda_hrs,scl_hrs;




static bool twi_master_clear_bus(void);
static bool twi_master_issue_startcondition(void);
static bool twi_master_issue_stopcondition(void);
static bool twi_master_clock_byte(uint_fast8_t databyte);
static bool twi_master_clock_byte_in(uint8_t * databyte, bool ack);
static bool twi_master_wait_while_scl_low(void);

bool hrs_iic_init(uint8_t sda_pin,uint8_t scl_pin)
{
    // Configure both pins to output Standard 0, No-drive (open-drain) 1

    sda_hrs =sda_pin;
    scl_hrs=scl_pin;
	
    TWI_SDA_STANDARD0_NODRIVE1_HRS();
    TWI_SCL_STANDARD0_NODRIVE1_HRS(); /*lint !e416 "Creation of out of bounds pointer" */

    // Configure SCL as output
    TWI_SCL_HIGH_HRS();
    TWI_SCL_OUTPUT_HRS();

    // Configure SDA as output
    TWI_SDA_HIGH_HRS();
    TWI_SDA_OUTPUT_HRS();

    return twi_master_clear_bus();
}

bool hrs_iic_transfer(uint8_t address, uint8_t * data, uint16_t data_length, bool issue_stop_condition)
{
    bool transfer_succeeded = true;


    transfer_succeeded &= twi_master_issue_startcondition();
    transfer_succeeded &= twi_master_clock_byte(address);

    if (address & TWI_READ_BIT_HRS)
    {
        /* Transfer direction is from Slave to Master */
        while (data_length-- && transfer_succeeded)
        {
            // To indicate to slave that we've finished transferring last data byte
            // we need to NACK the last transfer.
            if (data_length == 0)
            {
                transfer_succeeded &= twi_master_clock_byte_in(data, (bool)false);
            }
            else
            {
                transfer_succeeded &= twi_master_clock_byte_in(data, (bool)true);
            }
            data++;
        }
    }
    else
    {
        /* Transfer direction is from Master to Slave */
        while (data_length-- && transfer_succeeded)
        {
            transfer_succeeded &= twi_master_clock_byte(*data);
            data++;
        }
    }

    if (issue_stop_condition || !transfer_succeeded)
    {
        transfer_succeeded &= twi_master_issue_stopcondition();
    }

    return transfer_succeeded;
}

/**
 * @brief Function for detecting stuck slaves and tries to clear the bus.
 *
 * @return
 * @retval false Bus is stuck.
 * @retval true Bus is clear.
 */
static bool twi_master_clear_bus(void)
{
    bool bus_clear;
    uint_fast8_t i;
    TWI_SDA_HIGH_HRS();
    TWI_SCL_HIGH_HRS();
    TWI_DELAY_HRS();


    if (TWI_SDA_READ_HRS() == 1 && TWI_SCL_READ_HRS() == 1)
    {
        bus_clear = true;
    }
    else if (TWI_SCL_READ_HRS() == 1)
    {
        bus_clear = false;
        // Clock max 18 pulses worst case scenario(9 for master to send the rest of command and 9 for slave to respond) to SCL line and wait for SDA come high
        for ( i = 18; i--;)
        {
            TWI_SCL_LOW_HRS();
            TWI_DELAY_HRS();
            TWI_SCL_HIGH_HRS();
            TWI_DELAY_HRS();

            if (TWI_SDA_READ_HRS() == 1)
            {
                bus_clear = true;
                break;
            }
        }
    }
    else
    {
        bus_clear = false;
    }

    return bus_clear;
}

/**
 * @brief Function for issuing TWI START condition to the bus.
 *
 * START condition is signaled by pulling SDA low while SCL is high. After this function SCL and SDA will be low.
 *
 * @return
 * @retval false Timeout detected
 * @retval true Clocking succeeded
 */
static bool twi_master_issue_startcondition(void)
{


    // Make sure both SDA and SCL are high before pulling SDA low.
    TWI_SDA_HIGH_HRS();
    TWI_DELAY_HRS();
    if (!twi_master_wait_while_scl_low())
    {
        return false;
    }

    TWI_SDA_LOW_HRS();
    TWI_DELAY_HRS();

    // Other module function expect SCL to be low
    TWI_SCL_LOW_HRS();
    TWI_DELAY_HRS();

    return true;
}

/**
 * @brief Function for issuing TWI STOP condition to the bus.
 *
 * STOP condition is signaled by pulling SDA high while SCL is high. After this function SDA and SCL will be high.
 *
 * @return
 * @retval false Timeout detected
 * @retval true Clocking succeeded
 */
static bool twi_master_issue_stopcondition(void)
{


    TWI_SDA_LOW_HRS();
    TWI_DELAY_HRS();
    if (!twi_master_wait_while_scl_low())
    {
        return false;
    }

    TWI_SDA_HIGH_HRS();
    TWI_DELAY_HRS();

    return true;
}

/**
 * @brief Function for clocking one data byte out and reads slave acknowledgment.
 *
 * Can handle clock stretching.
 * After calling this function SCL is low and SDA low/high depending on the
 * value of LSB of the data byte.
 * SCL is expected to be output and low when entering this function.
 *
 * @param databyte Data byte to clock out.
 * @return
 * @retval true Slave acknowledged byte.
 * @retval false Timeout or slave didn't acknowledge byte.
 */
static bool twi_master_clock_byte(uint_fast8_t databyte)
{
    uint_fast8_t i;
    bool transfer_succeeded = true;

    /** @snippet [TWI SW master write] */
    // Make sure SDA is an output
    TWI_SDA_OUTPUT_HRS();

    // MSB first
    for ( i = 0x80; i != 0; i >>= 1)
    {
        TWI_SCL_LOW_HRS();
        TWI_DELAY_HRS();

        if (databyte & i)
        {
            TWI_SDA_HIGH_HRS();
        }
        else
        {
            TWI_SDA_LOW_HRS();
        }

        if (!twi_master_wait_while_scl_low())
        {
            transfer_succeeded = false; // Timeout
            break;
        }
    }

    // Finish last data bit by pulling SCL low
    TWI_SCL_LOW_HRS();
    TWI_DELAY_HRS();

    /** @snippet [TWI SW master write] */

    // Configure TWI_SDA pin as input for receiving the ACK bit
    TWI_SDA_INPUT_HRS();

    // Give some time for the slave to load the ACK bit on the line
    TWI_DELAY_HRS();

    // Pull SCL high and wait a moment for SDA line to settle
    // Make sure slave is not stretching the clock
    transfer_succeeded &= twi_master_wait_while_scl_low();

    // Read ACK/NACK. NACK == 1, ACK == 0
    transfer_succeeded &= !(TWI_SDA_READ_HRS());

    // Finish ACK/NACK bit clock cycle and give slave a moment to release control
    // of the SDA line
    TWI_SCL_LOW_HRS();
    TWI_DELAY_HRS();

    // Configure TWI_SDA pin as output as other module functions expect that
    TWI_SDA_OUTPUT_HRS();

    return transfer_succeeded;
}


/**
 * @brief Function for clocking one data byte in and sends ACK/NACK bit.
 *
 * Can handle clock stretching.
 * SCL is expected to be output and low when entering this function.
 * After calling this function, SCL is high and SDA low/high depending if ACK/NACK was sent.
 *
 * @param databyte Data byte to clock out.
 * @param ack If true, send ACK. Otherwise send NACK.
 * @return
 * @retval true Byte read succesfully
 * @retval false Timeout detected
 */
static bool twi_master_clock_byte_in(uint8_t *databyte, bool ack)
{
    uint_fast8_t i;
    uint_fast8_t byte_read          = 0;
    bool         transfer_succeeded = true;

    /** @snippet [TWI SW master read] */
    // Make sure SDA is an input
    TWI_SDA_INPUT_HRS();

    // SCL state is guaranteed to be high here

    // MSB first
    for ( i = 0x80; i != 0; i >>= 1)
    {
        if (!twi_master_wait_while_scl_low())
        {
            transfer_succeeded = false;
            break;
        }

        if (TWI_SDA_READ_HRS())
        {
            byte_read |= i;
        }
        else
        {
            // No need to do anything
        }

        TWI_SCL_LOW_HRS();
        TWI_DELAY_HRS();
    }

    // Make sure SDA is an output before we exit the function
    TWI_SDA_OUTPUT_HRS();
    /** @snippet [TWI SW master read] */

    *databyte = (uint8_t)byte_read;

    // Send ACK bit

    // SDA high == NACK, SDA low == ACK
    if (ack)
    {

        TWI_SDA_LOW_HRS();
    }
    else
    {

        TWI_SDA_HIGH_HRS();
    }

    // Let SDA line settle for a moment
    TWI_DELAY_HRS();

    // Drive SCL high to start ACK/NACK bit transfer
    // Wait until SCL is high, or timeout occurs
    if (!twi_master_wait_while_scl_low())
    {
        transfer_succeeded = false; // Timeout
    }

    // Finish ACK/NACK bit clock cycle and give slave a moment to react

    TWI_SCL_LOW_HRS();
    TWI_DELAY_HRS();

    return transfer_succeeded;
}

#include "nrf_delay.h"
/**
 * @brief Function for pulling SCL high and waits until it is high or timeout occurs.
 *
 * SCL is expected to be output before entering this function.
 * @note If TWI_MASTER_TIMEOUT_COUNTER_LOAD_VALUE is set to zero, timeout functionality is not compiled in.
 * @return
 * @retval true SCL is now high.
 * @retval false Timeout occurred and SCL is still low.
 */
static bool twi_master_wait_while_scl_low(void)
{
#if TWI_MASTER_TIMEOUT_COUNTER_LOAD_VALUE_HRS != 0
    uint32_t volatile timeout_counter = TWI_MASTER_TIMEOUT_COUNTER_LOAD_VALUE_HRS;
#endif

    // Pull SCL high just in case if something left it low
    TWI_SCL_HIGH_HRS();
    TWI_DELAY_HRS();

    while (TWI_SCL_READ_HRS() == 0)
    {
        // If SCL is low, one of the slaves is busy and we must wait
        
#if TWI_MASTER_TIMEOUT_COUNTER_LOAD_VALUE_HRS != 0
		IIC_DELAY(20);
		
//		TWI_DELAY_HRS();
        if (timeout_counter-- == 0)
        {
			
            // If timeout_detected, return false
            return false;
        }
#endif
    }

    return true;
}

/*lint --flb "Leave library region" */
