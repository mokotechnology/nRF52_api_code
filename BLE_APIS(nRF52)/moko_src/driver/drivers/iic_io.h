
#ifndef IIC_H
#define IIC_H


#include <stdbool.h>
#include <stdint.h>
#include "nrf_gpio.h"


/*lint -e415 -e845 -save "Out of bounds access" */
#define TWI_SDA_STANDARD0_NODRIVE1_HRS() do { \
        NRF_GPIO->PIN_CNF[sda_hrs] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) \
        |(GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos)    \
        |(GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)  \
        |(GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) \
        |(GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);  \
} while (0) /*!< Configures SDA pin to Standard-0, No-drive 1 */


#define TWI_SCL_STANDARD0_NODRIVE1_HRS() do { \
        NRF_GPIO->PIN_CNF[scl_hrs] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) \
        |(GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos)    \
        |(GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)  \
        |(GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) \
        |(GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);  \
} while (0) /*!< Configures SCL pin to Standard-0, No-drive 1 */


#define TWI_SCL_HIGH_HRS()   do { NRF_GPIO->OUTSET = (1UL << scl_hrs); } while(0)   /*!< Pulls SCL line high */
#define TWI_SCL_LOW_HRS()    do { NRF_GPIO->OUTCLR = (1UL << scl_hrs); } while(0)   /*!< Pulls SCL line low  */
#define TWI_SDA_HIGH_HRS()   do { NRF_GPIO->OUTSET = (1UL << sda_hrs);  } while(0)   /*!< Pulls SDA line high */
#define TWI_SDA_LOW_HRS()    do { NRF_GPIO->OUTCLR = (1UL << sda_hrs);  } while(0)   /*!< Pulls SDA line low  */
#define TWI_SDA_INPUT_HRS()  do { NRF_GPIO->DIRCLR = (1UL << sda_hrs);  } while(0)   /*!< Configures SDA pin as input  */
#define TWI_SDA_OUTPUT_HRS() do { NRF_GPIO->DIRSET = (1UL << sda_hrs);  } while(0)   /*!< Configures SDA pin as output */
#define TWI_SCL_OUTPUT_HRS() do { NRF_GPIO->DIRSET = (1UL << scl_hrs); } while(0)   /*!< Configures SCL pin as output */


#define TWI_SDA_READ_HRS() ((NRF_GPIO->IN >> sda_hrs) & 0x1UL)                     /*!< Reads current state of SDA */
#define TWI_SCL_READ_HRS() ((NRF_GPIO->IN >> scl_hrs) & 0x1UL)                    /*!< Reads current state of SCL */



#define TWI_READ_BIT_HRS                 (0x01)        //!< If this bit is set in the address field, transfer direction is from slave to master.
#define TWI_ISSUE_STOP_HRS               ((bool)true)  //!< Parameter for @ref twi_master_transfer
#define TWI_DONT_ISSUE_STOP_HRS          ((bool)false) //!< Parameter for @ref twi_master_transfer



#define HRS_US   1
#define TWI_MASTER_TIMEOUT_COUNTER_LOAD_VALUE_HRS   20//1000


#define TWI_ISSUE_STOP               ((bool)true)  //!< Parameter for @ref twi_master_transfer
#define TWI_DONT_ISSUE_STOP          ((bool)false) //!< Parameter for @ref twi_master_transfer



bool hrs_iic_init(uint8_t sda_pin,uint8_t scl_pin);

// bool iic_transfer(uint8_t address, uint8_t *data, uint16_t data_length, bool issue_stop_condition);
bool hrs_iic_transfer(uint8_t address, uint8_t * data, uint16_t data_length, bool issue_stop_condition);

#endif //TWI_MASTER_H
