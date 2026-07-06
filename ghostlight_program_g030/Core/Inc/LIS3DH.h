#ifndef LIS3DH_H_
#define LIS3DH_H_

#include "main.h"

/**
 * @brief run initialization sequence for LIS3DH and verify device identity
 *
 * @return 0 -> all is ok; 1 -> I2C error; 2 -> LIS3DH internal/identity error
 */
uint8_t LIS3DH_init(void);

/**
 * @brief get raw accelerometer data for X, Y, and Z axes
 *
 * @output x, y, z pointers to receive 16-bit signed accelerometer values
 * @return 0 -> all is ok; 1 -> I2C error
 */
uint8_t LIS3DH_getAxes(int16_t *x, int16_t *y, int16_t *z);


/**
 * @brief read one specific register of LIS3DH using your shared I2C DMA pattern
 *
 * @input reg, register to read
 * @output buffer, pointer to buffer with output
 * @input size of the buffer
 * @return error, if any occured
 */
uint8_t LIS3DH_read_reg(uint8_t reg, uint8_t *buffer, uint16_t size);
#endif /* LIS3DH_H_ */
