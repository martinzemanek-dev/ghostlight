/*
 * LIS3DH.c
 *
 *  Created on: 6. 7. 2026
 *      Author: mobil
 */

#include "LIS3DH.h"
#include "i2c.h"

// Potential 7-bit base addresses due to floating SA0 pin
#define LIS3DH_ADDR_0           0x18
#define LIS3DH_ADDR_1           0x19

// Dynamic runtime address holder
static uint8_t lis3dh_active_write_addr = (LIS3DH_ADDR_1 << 1);
static uint8_t lis3dh_active_read_addr = ((LIS3DH_ADDR_1 << 1) | 0x01);

//  ---- REGISTERS ----
#define LIS3DH_REG_WHO_AM_I     0x0F
#define LIS3DH_REG_CTRL_REG1    0x20
#define LIS3DH_REG_OUT_X_L      0x28
#define LIS3DH_REG_CTRL_REG3    0x22
#define LIS3DH_REG_CLICK_CFG    0x38
#define LIS3DH_REG_CLICK_THS    0x3A
#define LIS3DH_REG_TIME_LIMIT   0x3B
#define LIS3DH_REG_TIME_LATENCY 0x3C
#define LIS3DH_REG_TIME_WINDOW  0x3D
#define LIS3DH_REG_INT1_CFG     0x30
#define LIS3DH_REG_INT1_THS     0x32
#define LIS3DH_REG_INT1_DURATION 0x33

// Auto-increment bitmask required by ST chips for reading multiple bytes
#define LIS3DH_AUTO_INCREMENT   0x80

#define LIS3DH_READ_REQ_MSG_SIZE 1

/**
 * @brief read one specific register of LIS3DH using your shared I2C DMA pattern
 */
uint8_t LIS3DH_read_reg(uint8_t reg, uint8_t *buffer, uint16_t size) {
	static uint8_t selectRegBuffer[LIS3DH_READ_REQ_MSG_SIZE];

	while (i2c_busy)
		__WFI();

	i2c_busy = 1;
	selectRegBuffer[0] = reg;

	if (HAL_I2C_Master_Transmit_DMA(&hi2c1, lis3dh_active_write_addr,
			selectRegBuffer,
			LIS3DH_READ_REQ_MSG_SIZE) != HAL_OK) {
		i2c_busy = 0;
		return 1;
	}

	HAL_Delay(1);

	while (i2c_busy) {
		__WFI();
	}

	i2c_busy = 1;

	if (HAL_I2C_Master_Receive_DMA(&hi2c1, lis3dh_active_read_addr, buffer,
			size) != HAL_OK) {
		i2c_busy = 0;
		return 1;
	}

	while (i2c_busy) {
		__WFI();
	}

	return i2c_err;
}

#define LIS3DH_REG_MSG_SIZE 2
/**
 * @brief write one specific register of LIS3DH using your shared I2C DMA pattern
 */
uint8_t LIS3DH_write_reg(uint8_t reg, uint8_t value) {
	static uint8_t transmitBuff[LIS3DH_REG_MSG_SIZE];

	while (i2c_busy)
		__WFI();

	i2c_busy = 1;
	transmitBuff[0] = reg;
	transmitBuff[1] = value;

	if (HAL_I2C_Master_Transmit_DMA(&hi2c1, lis3dh_active_write_addr,
			transmitBuff,
			LIS3DH_REG_MSG_SIZE) != HAL_OK) {
		i2c_busy = 0;
		return 1;
	}

	while (i2c_busy)
		__WFI();

	return i2c_err;
}

uint8_t LIS3DH_init(void) {
	uint8_t identity_buffer = 0;
	uint8_t address_found = 0;

	// Scan Address 1 (0x19)
	lis3dh_active_write_addr = (LIS3DH_ADDR_1 << 1);
	lis3dh_active_read_addr = ((LIS3DH_ADDR_1 << 1) | 0x01);
	if (LIS3DH_read_reg(LIS3DH_REG_WHO_AM_I, &identity_buffer, 1) == 0
			&& identity_buffer == 0x33) {
		address_found = 1;
	}

	// Scan Address 0 (0x18) if Address 1 failed
	if (!address_found) {
		lis3dh_active_write_addr = (LIS3DH_ADDR_0 << 1);
		lis3dh_active_read_addr = ((LIS3DH_ADDR_0 << 1) | 0x01);
		if (LIS3DH_read_reg(LIS3DH_REG_WHO_AM_I, &identity_buffer, 1) == 0
				&& identity_buffer == 0x33) {
			address_found = 1;
		}
	}

	// If neither configuration returned 0x33 identity signature, report error
	if (!address_found) {
		return 2;
	}
	HAL_Delay(1);

	//Rotation trigger?
	// 1. ODR = 25Hz (0x37). Orientation tracking doesn't need 400Hz speed.
	// 25Hz saves immense battery life!
	LIS3DH_write_reg(LIS3DH_REG_CTRL_REG1, 0x37);
	HAL_Delay(1);

	// 2. Route the Inertial Interrupt 1 (IA1) to physical INT1 Pin (Bit 6 = I1_IA1)
	LIS3DH_write_reg(LIS3DH_REG_CTRL_REG3, 0x40);
	HAL_Delay(1);

	// 3. Clear out old tap settings entirely
	LIS3DH_write_reg(LIS3DH_REG_CLICK_CFG, 0x00);
	HAL_Delay(1);

	// 4. Set 6D Configuration (0xC0 = 6D Position Mode + enable high/low on all axes)
	// This tells the sensor to trigger an interrupt whenever the dominant gravity axis changes.
	LIS3DH_write_reg(LIS3DH_REG_INT1_CFG, 0xC0 | 0x3F);
	HAL_Delay(1);

	// 5. Set Threshold (0x37 = ~60 degree tilt threshold cone)
	LIS3DH_write_reg(LIS3DH_REG_INT1_THS, 0x37);
	HAL_Delay(1);

	// 6. Set Duration (0x05 = Must stay upside down for 5 samples before firing)
	// At 25Hz, 5 samples is a 200ms debounce filter so quick accidental shakes are ignored!
	LIS3DH_write_reg(LIS3DH_REG_INT1_DURATION, 0x05);
	HAL_Delay(1);
	return 0;
}

uint8_t LIS3DH_getAxes(int16_t *x, int16_t *y, int16_t *z) {
	static uint8_t raw_data[6];

// Read 6 bytes sequentially using the LIS3DH auto-increment bitmask
	uint8_t err = LIS3DH_read_reg(LIS3DH_REG_OUT_X_L | LIS3DH_AUTO_INCREMENT,
			raw_data, 6);
	if (err != 0) {
		return 1;
	}

// Unpack data registers into signed 16-bit variables
	*x = (int16_t) ((raw_data[1] << 8) | raw_data[0]);
	*y = (int16_t) ((raw_data[3] << 8) | raw_data[2]);
	*z = (int16_t) ((raw_data[5] << 8) | raw_data[4]);

	return 0;
}
