#ifndef FLASH
#define FLASH

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stdint.h"
#include "string.h"

#define DATA_START_ADDRESS 		 	((uint32_t)0x0801FC00)	//Page 127

void 	Flash_Lock(void);
void 	Flash_Unlock(void);
//void 	Flash_Erase(uint32_t addr);
void Flash_Erase_Sector(uint32_t Sector);
void EEPROM_init(uint16_t size);
void EEP_write(uint16_t add, uint16_t data);
uint16_t Flash_read(uint16_t add);

#endif