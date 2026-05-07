/*
 * flash.cpp
 *
 *  Created on: 6. 5. 2026
 *      Author: mobil
 */

#include "flash.h"
#include <string.h>

struct PersistentData resetData;

uint32_t initPersistentData() {
	if (resetData.checkNumber != CHECK_NUMBER) {
		struct PersistentData *flashDataPtr = (struct PersistentData*) SETTINGS_FLASH_ADDR;

		if (flashDataPtr->checkNumber != CHECK_NUMBER) {
			resetData.restartCount = 0;
			resetData.checkNumber = CHECK_NUMBER;
		} else {
			resetData.restartCount = (flashDataPtr->restartCount + 1);
			resetData.checkNumber = CHECK_NUMBER;
		}

		return storePersistentDataToFlash(&resetData);
	}
	return HAL_OK;
}

uint32_t getCycleCount() {
	return resetData.restartCount;
}

uint32_t storePersistentDataToFlash(struct PersistentData *dataToStore) {
	HAL_FLASH_Unlock();

	FLASH_EraseInitTypeDef eraseInit;
	uint32_t pageError;
	eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	eraseInit.Page = 7; // Assuming 16KB device, last 2KB page
	eraseInit.NbPages = 1;

	// Note: HAL_FLASHEx_Erase is a blocking call
	uint32_t err = HAL_FLASHEx_Erase(&eraseInit, &pageError);
	if (err == HAL_OK) {
		uint64_t doubleWord;
		memcpy(&doubleWord, dataToStore, sizeof(uint64_t));
		err = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, SETTINGS_FLASH_ADDR,
				doubleWord);
	}

	HAL_FLASH_Lock();

	return err;
}
