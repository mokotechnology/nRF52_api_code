
#include "boot.h"



 void flash_nvic_page_erase(uint32_t page_address)
{
	// Turn on flash erase enable and wait until the NVMC is ready:
	NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);
	while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	{
		// Do nothing.
	} 
	// Erase page:
	NRF_NVMC->ERASEPAGE = page_address;
	while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	{
		// Do nothing.
	} 
	// Turn off flash erase enable and wait until the NVMC is ready:
	NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
	while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	{
		// Do nothing.
	}
} 



void flash_nvic_word_write(uint32_t *address, uint32_t value)
{
	// Turn on flash write enable and wait until the NVMC is ready:
	NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);
	while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	{
		// Do nothing.
	} 
	*address = value;
	while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	{
		// Do nothing.
	} 
	// Turn off flash write enable and wait until the NVMC is ready:
	NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
	while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	{
		// Do nothing.
	}
} 


void EraseBoot(void)
{

	// Turn on flash erase enable and wait until the NVMC is ready:
	NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);
	while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	{
		// Do nothing.
	} 
	// Erase page:
	NRF_NVMC->ERASEUICR = 1;
	while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	{
		// Do nothing.
	} 
	// Turn off flash erase enable and wait until the NVMC is ready:
	NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
	while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	{
		// Do nothing.
	}

}




void boot_to_new_appilacation(uint8_t* datain,uint8_t len)
{

	uint32_t buf[4];

	uint32_t value;

	value=*((uint32_t*)BOOT_PARA_ADDR);
	buf[0]=BOOT_ADDR;
	if(12==len)
	{
		EraseBoot();
		memcpy((uint8_t*)&buf[1],datain,12);
		flash_nvic_word_write((uint32_t*)BOOT_PARA_ADDR,buf[0]);
		flash_nvic_word_write((uint32_t*)(BOOT_PARA_CUSTOM_ADDR+0),buf[1]);
		flash_nvic_word_write((uint32_t*)(BOOT_PARA_CUSTOM_ADDR+4),buf[2]);
		flash_nvic_word_write((uint32_t*)(BOOT_PARA_CUSTOM_ADDR+8),buf[3]);
	}else if(value!=BOOT_ADDR)
	{
		EraseBoot();
		flash_nvic_word_write((uint32_t*)BOOT_PARA_ADDR,buf[0]);
	}


}



