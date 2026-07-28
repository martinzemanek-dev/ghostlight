#include "LP5817.h"
#include "i2c.h"

#define LP5817_ADDR 		0x2D
#define LP5817_ADDR_WRITE 	(LP5817_ADDR << 1)
#define LP5817_ADDR_READ 	(LP5817_ADDR << 1) | 0x01

//	---- REGISTERS ----
#define LP5817_REG_CHIP_EN 		0x00

//> CONFIG
#define LP5817_REG_DEV_CONFIG_0 0x01
#define LP5817_REG_DEV_CONFIG_1 0x02
#define LP5817_REG_DEV_CONFIG_2 0x03
#define LP5817_REG_DEV_CONFIG_3 0x04

//> CMD
#define LP5817_REG_SHUTDOWN_CMD 0x0D
#define LP5817_REG_RESET_CMD 	0x0E
#define LP5817_REG_UPDATE_CMD 	0x0F
#define LP5817_REG_FLAG_CLR_CMD 0x13

#define LP5817_REG_OUT0_DC 		0x14
#define LP5817_REG_OUT1_DC 		0x15
#define LP5817_REG_OUT2_DC 		0x16

#define LP5817_REG_OUT0_MAN_PWM 0x18
#define LP5817_REG_OUT1_MAN_PWM 0x19
#define LP5817_REG_OUT2_MAN_PWM 0x1A

#define LP5817_REG_FLAG			0x40

//	---- CODE ----

#define LP5817_READ_REQ_MSG_SIZE 1
/*
 * Should start by sending address with fliped directional bit and register and than wait for bytes with register number + received data
 */
uint8_t LP5817_read_reg(uint8_t reg, uint8_t *buffer) {
	static uint8_t selectRegBuffer[LP5817_READ_REQ_MSG_SIZE];

	while (i2c_busy)
		__WFI();

	i2c_busy = 1;
	selectRegBuffer[0] = reg;

	// 2. Transmit the register address using DMA
	if (HAL_I2C_Master_Transmit_DMA(&hi2c1, LP5817_ADDR_WRITE, selectRegBuffer,
			LP5817_READ_REQ_MSG_SIZE) != HAL_OK) {
		i2c_busy = 0;
		return 1; // Likely returns HAL_BUSY here if the bus is stuck
	}

	HAL_Delay(1);

	// 3. Wait for DMA to finish (using your flag)
	while (i2c_busy) {
		__WFI();
	}
	i2c_busy = 1;
	// 4. Mandatory Bus Free Time (t_BUF)
	// The LP5817 needs time to process the address before the restart

	// 5. Receive the data byte
	if (HAL_I2C_Master_Receive_DMA(&hi2c1, LP5817_ADDR_READ, buffer, 1)
			!= HAL_OK) {
		return 1;
	}

	while (i2c_busy) {
		__WFI();
	}

	return 0;
}

#define LP5817_REG_MSG_SIZE 2
/**
 * @brief write one specific register of LP5817
 *
 * @input reg, number of the register in LP5817
 * @input value, value to write into the register
 */
uint8_t LP5817_write_reg(uint8_t reg, uint8_t value) {
	static uint8_t transmitBuff[LP5817_REG_MSG_SIZE];

	while (i2c_busy)
		__WFI();

	i2c_busy = 1;
	transmitBuff[0] = reg;
	transmitBuff[1] = value;

	if (HAL_I2C_Master_Transmit_DMA(&hi2c1, LP5817_ADDR_WRITE, transmitBuff,
	LP5817_REG_MSG_SIZE) != HAL_OK) {
		i2c_busy = 0;
		return 1;
	}

	while (i2c_busy)
		__WFI();

	return 0;
}

uint8_t LP5817_read_flags() {
	//read flags to get if all is ok
	static uint8_t flag_data = 0;
	if (0 != LP5817_read_reg(LP5817_REG_FLAG, &flag_data))
		return 1;
	if (flag_data != 0)
		return (flag_data << 2) | 1;
	return 0;
}

uint8_t LP5817_read_flags_with_reset()
{
	int err = LP5817_read_flags();

	for (int i = 0;(i < 10) && (err != 0);i++)
	{
		HAL_Delay(1);
		//reset bits
		LP5817_write_reg(LP5817_REG_FLAG_CLR_CMD, 0x03);
		HAL_Delay(1);
		err = LP5817_read_flags();
	}
	return err;

}
uint8_t LP5817_init() {
	//Reset chip
	int err = LP5817_write_reg(LP5817_REG_RESET_CMD, 0xCC);
	if (err != HAL_OK)
		return err;
	HAL_Delay(1);

	//Enable chip
	err = LP5817_write_reg(LP5817_REG_CHIP_EN, 0x01);
	if (err != HAL_OK)
		return err;
	HAL_Delay(1);

	//set global current limiter to 25.5mA
	err = LP5817_write_reg(LP5817_REG_DEV_CONFIG_0, 0x00);
	if (err != HAL_OK)
		return err;
	HAL_Delay(1);

	//set individual limiters
	// X to 20mA
	err = LP5817_write_reg(LP5817_REG_OUT0_DC, 0xC8);
	if (err != HAL_OK)
		return err;
	HAL_Delay(1);

	// X to 20mA
	err = LP5817_write_reg(LP5817_REG_OUT1_DC, 0xC8);
	if (err != HAL_OK)
		return err;
	HAL_Delay(1);

	// X to 20mA
	err = LP5817_write_reg(LP5817_REG_OUT2_DC, 0xC8);
	if (err != HAL_OK)
		return err;
	HAL_Delay(1);

	//enable all channels
	err = LP5817_write_reg(LP5817_REG_DEV_CONFIG_1, 0x07);
	if (err != HAL_OK)
		return err;
	HAL_Delay(1);

	//Update chip
	err = LP5817_write_reg(LP5817_REG_UPDATE_CMD, 0x55);
	if (err != HAL_OK)
		return err;
	HAL_Delay(1);

	static uint8_t verifyBuff = 0;

	err = LP5817_read_reg(LP5817_REG_CHIP_EN, &verifyBuff);
	if (err != HAL_OK)
		return err;
	HAL_Delay(1);

	return LP5817_read_flags_with_reset();
}

uint8_t LP5817_setColor(uint8_t r, uint8_t g, uint8_t b) {
	if (0 != LP5817_write_reg(LP5817_REG_OUT0_MAN_PWM, r)) {
		return 1;
	}
	HAL_Delay(1);

	if (0 != LP5817_write_reg(LP5817_REG_OUT1_MAN_PWM, g)) {
		return 1;
	}
	HAL_Delay(1);

	if (0 != LP5817_write_reg(LP5817_REG_OUT2_MAN_PWM, b)) {
		return 1;
	}
	HAL_Delay(1);

	return LP5817_read_flags();
}

