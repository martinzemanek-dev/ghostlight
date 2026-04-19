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

/*
 * Should start by sending address with fliped directional bit and register and than wait for bytes with register number + received data
 */
uint8_t LP5817_read_reg(uint8_t reg, uint8_t *buffer)
{
	while (i2c_busy)__WFI();

	if (HAL_I2C_Mem_Read(&hi2c1, LP5817_ADDR_READ, reg, I2C_MEMADD_SIZE_8BIT, buffer, 1, 100))
		return 1;
	return 0;
}


#define LP5817_REG_MSG_SIZE 2
/**
 * @brief write one specific register of LP5817
 *
 * @input reg, number of the register in LP5817
 * @input value, value to write into the register
 */
uint8_t LP5817_write_reg(uint8_t reg, uint8_t value)
{
	static uint8_t transmitBuff [LP5817_REG_MSG_SIZE];

	while (i2c_busy)
		__WFI();


	transmitBuff[0] = reg;
	transmitBuff[1] = value;

	if (HAL_I2C_Master_Transmit_DMA(&hi2c1, LP5817_ADDR_WRITE, transmitBuff, LP5817_REG_MSG_SIZE) != HAL_OK)
	{
		i2c_busy = 0;
		return 1;
	}

	while (i2c_busy)
		__WFI();

	return 0;
}

uint8_t LP5817_init()
{
	//Reset chip
	if (0 != LP5817_write_reg(LP5817_REG_RESET_CMD, 0xCC))
			return 1;

	HAL_Delay(10);

	//Enable chip
	if (0 != LP5817_write_reg(LP5817_REG_CHIP_EN, 0x01))
		return 1;

	//set global current limiter to 25.5mA
	if (0 != LP5817_write_reg(LP5817_REG_DEV_CONFIG_0, 0x00))
		return 1;

	//set individual limiters
	// X to 20mA
	if (0 != LP5817_write_reg(LP5817_REG_OUT0_DC, 0xC8))
		return 1;


	// X to 20mA
	if (0 != LP5817_write_reg(LP5817_REG_OUT1_DC, 0xC8))
		return 1;

	// X to 20mA
	if (0 != LP5817_write_reg(LP5817_REG_OUT2_DC, 0xC8))
		return 1;

	//enable all channels
	if (0 != LP5817_write_reg(LP5817_REG_DEV_CONFIG_1, 0x07))
		return 1;

	//Update chip
	if (0 != LP5817_write_reg(LP5817_REG_UPDATE_CMD, 0x55))
		return 1;

	//read flags to get if all is ok
	uint8_t flag_data = 0;
	if(0 != LP5817_read_reg(LP5817_REG_FLAG, &flag_data))
		return 1;
	if (flag_data != 0)
		return 2;
	return 0;
}

uint8_t LP5817_setColor(uint8_t r, uint8_t g, uint8_t b)
{
	if (0 != LP5817_write_reg(LP5817_REG_OUT0_MAN_PWM, r))
			return 1;

	if (0 != LP5817_write_reg(LP5817_REG_OUT1_MAN_PWM, g))
			return 1;

	if (0 != LP5817_write_reg(LP5817_REG_OUT2_MAN_PWM, b))
			return 1;

	//read flags to get if all is ok
	uint8_t flag_data = 0;
	if(0 != LP5817_read_reg(LP5817_REG_FLAG, &flag_data))
		return 1;
	if (flag_data != 0)
		return 2;

	return 0;
}



