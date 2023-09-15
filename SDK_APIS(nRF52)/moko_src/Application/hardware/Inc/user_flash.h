#ifndef user_flash_h_
#define user_flash_h_



#include "ble_gap.h"

#include "stdint.h"




#define HIS_HEAD 0xe7
#define FACTORY_SET_HEAD (0xaaaaaa00+HIS_HEAD)




#define MAX_MANU_NAME_LEN 30
#define MAX_SOFTWARE_LEN   20


/*******************************************/
//#pragma pack(4)
typedef struct
{	
	int8_t Beacon_rssi;
	
	uint16_t adv_interval;
	int8_t  tx_Power;
	
	uint8_t device_name[10];
	uint8_t device_name_len;
	
	uint8_t mac_addr[6];
	
	uint8_t manufactur_name_str[30];
	uint8_t product_type_str[20];
	uint8_t software_verison_str[20];
//	uint8_t hardware_verison_str[20];
	uint8_t produce_time[4];

    uint16_t tof_interval;/*Time interval for updating distance results
                            range[1,65535],unit:s*/
    uint16_t tof_time;/*The sampling time of per tof_times
                        range[8,140]*/
    uint16_t tof_times;/*The number of samples per period. 
                         range[2,65535]*/
    uint16_t tof_mode;/*can choose Long range mode(0x02)and Short range mode(0x01)
                        The Stort range mode is low-power mode, it can measure the 
                        max distance at about 1200mm.
                        The Long range mode can measure the max distance at about 
                        3000mm*/
	
}STU_HIS;
//#pragma pack()

extern STU_HIS StuHis;





void read_user_flash(void);
void write_user_flash(uint8_t mode);

void read_factory_flash(void);

void task_save_flash(void);
void save_user_flash(void);
void ask_reset_his(void);
void ask_save_factory(void);

void write_factory_flash(uint32_t *pdata);
#endif
