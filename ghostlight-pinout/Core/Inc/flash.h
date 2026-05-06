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

class FlashDataManager
{

public:
struct PersistentData
{
	uint32_t restartCount;
	uint32_t checkNumber; //detect empty flash or bad read
};

/**
 * @brief load and modify all data upon reset
 *
 * @return uint32_t count of restars stored in
 */
uint32_t init();

/**
 * @brief get number of current restart cycle
 *
 * @return uin32_t number of current cycle
 */
uint32_t getCycleCount();

private:

/**
 * @brief sore data in this object to flash
 *
 * @return HAL failure, if it occured
 */
uint32_t storeDataToFlash();

PersistentData m_data;
};

#endif /* INC_FLASH_H_ */
