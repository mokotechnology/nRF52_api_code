#ifndef BOOT51822_H___
#define BOOT51822_H___
#include "stdint.h"
#include "string.h"

//#include "nrf52.h"
//#include "nrf52_bitfields.h"
//#include "in_flash_manage.h"



void boot_to_new_appilacation(uint8_t* datain,uint8_t len);

void flash_nvic_word_write(uint32_t *address, uint32_t value);

void flash_nvic_page_erase(uint32_t page_address);

#endif



