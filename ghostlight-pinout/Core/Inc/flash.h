/*
 * flash.h
 *
 *  Created on: 6. 5. 2026
 *      Author: mobil
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "stm32c0xx_hal.h"


// Define the start of your reserved page
#define SETTINGS_FLASH_ADDR 0x08003800
#define CHECK_NUMBER 0x55AA55AA


struct PersistentData
{
	uint32_t restartCount;
	uint32_t checkNumber; //detect empty flash or bad read
};

/**
 * @brief load and modify all data upon reset
 *
 * @return uint32_t error, which occured
 */
uint32_t initPersistentData();

uint32_t getCycleCount();

uint32_t storePersistentDataToFlash(struct PersistentData *dataToStore);

extern struct PersistentData resetData;

#endif /* INC_FLASH_H_ */
