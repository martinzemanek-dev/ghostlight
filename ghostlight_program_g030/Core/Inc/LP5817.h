
#include "main.h"

/**
 * @brief run initialization sequence for LP5817
 *
 * @return 0 -> all is ok; 1 -> I2C error; 2 -> LP5817 internal error
 */
uint8_t LP5817_init();


/**
 * @brief set channels of LP5817 to specific values
 *
 * @return 0 -> all is ok; 1 -> I2C error; 2 -> LP5817 internal error
 */
uint8_t LP5817_setColor(uint8_t r, uint8_t g, uint8_t b);
