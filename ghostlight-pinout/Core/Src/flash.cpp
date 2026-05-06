/*
 * flash.cpp
 *
 *  Created on: 6. 5. 2026
 *      Author: mobil
 */

#include "flash.h"

uint32_t FlashDataManager::init() {
	if (m_data.checkNumber != CHECK_NUMBER) {
		PersistentData *flashDataPtr = (PersistentData*) SETTINGS_FLASH_ADDR;

		if (flashDataPtr->checkNumber != CHECK_NUMBER) {
			m_data.restartCount = 0;
			m_data.checkNumber = CHECK_NUMBER;
		} else {
			m_data.restartCount = (flashDataPtr->restartCount + 1);
			m_data.checkNumber = CHECK_NUMBER;
		}

		return storeDataToFlash();
	}
	return HAL_OK;
}

uint32_t FlashDataManager::getCycleCount() {
	return m_data.restartCount;
}

uint32_t FlashDataManager::storeDataToFlash() {
	HAL_FLASH_Unlock();

	FLASH_EraseInitTypeDef eraseInit;
	uint32_t pageError;
	eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	eraseInit.Page = 7; // Assuming 16KB device, last 2KB page
	eraseInit.NbPages = 1;

	// Note: HAL_FLASHEx_Erase is a blocking call
	uint32_t err = HAL_FLASHEx_Erase(&eraseInit, &pageError)
	if (err == HAL_OK) {
		uint64_t doubleWord;
		std::memcpy(&doubleWord, &m_data, sizeof(uint64_t));
		err = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, SETTINGS_FLASH_ADDR,
				doubleWord);
	}

	HAL_FLASH_Lock();

	return err;
}
