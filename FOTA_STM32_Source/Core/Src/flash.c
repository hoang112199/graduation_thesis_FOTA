#include "flash.h"

uint16_t EEPROM_buffer[512];
uint16_t eeprom_size=512;

void EEPROM_init(uint16_t size)
{
	eeprom_size=size;
	for(int i=0;i<eeprom_size;i++)EEPROM_buffer[i]=Flash_read(i);
}
void Flash_Lock()
{
	HAL_FLASH_Lock();
}

void Flash_Unlock()
{
	HAL_FLASH_Unlock();
}
/*
void Flash_Erase(uint32_t addr)
{
Flash_Unlock();
  while((FLASH->SR&FLASH_SR_BSY));
  FLASH->CR |= FLASH_CR_PER; //Page Erase Set
  FLASH->AR = addr; //Page Address
  FLASH->CR |= FLASH_CR_STRT; //Start Page Erase
  while((FLASH->SR&FLASH_SR_BSY));
	FLASH->CR &= ~FLASH_SR_BSY;
  FLASH->CR &= ~FLASH_CR_PER; //Page Erase Clear
	
Flash_Lock();
}*/

void Flash_Erase_Sector(uint32_t Sector)
{
	HAL_StatusTypeDef status;
	
	// Unlock the Flash
	HAL_FLASH_Unlock();
	
	// Erase the specified sector
	FLASH_Erase_Sector(Sector, FLASH_VOLTAGE_RANGE_3);
	
	// Lock the Flash
	HAL_FLASH_Lock();
}


void EEP_write(uint16_t add, uint16_t data)
{
	EEPROM_buffer[add]=data;
}

uint16_t EEP_read(uint16_t add)
{
	return EEPROM_buffer[add];
}

uint16_t Flash_read(uint16_t add)
{
	int32_t addr;
	addr = DATA_START_ADDRESS + (add*2);
	uint16_t* val = (uint16_t *)addr;
	return *val;
}
