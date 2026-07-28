/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LP5817.h"
#include "LIS3DH.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile int mode = 0;
volatile int measure = 1;
volatile int change_mod = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void handleI2CErr(uint8_t error_code) {
	//HAL_GPIO_WritePin(LIVE_GPIO_Port, LIVE_Pin, RESET);
	// Check if any error occurred (Bit 0 is our 'Error Present' marker)
	if (error_code) {
		while (1) {
		}
	}
}

#define MAX_BRIGHT 100
#define SCALE(val) ((val * MAX_BRIGHT) / 255)

/**
 * @brief set color based on the hue(0-329)
 *
 * @param hue, number between 0 and 365, which chooses specific HUE
 */
void SetRainbowColor(uint16_t hue) {
	uint8_t r = 0, g = 0, b = 0;
	uint8_t region = hue / 60;
	uint8_t remainder = (hue % 60) * 255 / 60;

	uint8_t p = 0; // Matching 0% brightness base
	uint8_t q = 255 - remainder;
	uint8_t t = remainder;

	// Scale down from 255 to your maximum target brightness (e.g., max 150 to not blind you)

	switch (region) {
	case 0:
		r = 255;
		g = t;
		b = p;
		break;
	case 1:
		r = q;
		g = 255;
		b = p;
		break;
	case 2:
		r = p;
		g = 255;
		b = t;
		break;
	case 3:
		r = p;
		g = q;
		b = 255;
		break;
	case 4:
		r = t;
		g = p;
		b = 255;
		break;
	default:
		r = 255;
		g = p;
		b = q;
		break;
	}

	// Call your driver with the scaled relaxing colors
	LP5817_setColor(SCALE(r), SCALE(g), SCALE(b));
}

/**
 * @brief rutine, which cycles thru hues (0-359) in 30 ms intervals
 */
void rainbowRutine() {
	static uint16_t hue = 0;
	SetRainbowColor(hue);

	hue++;
	if (hue >= 360) {
		hue = 0;
	}

	HAL_Delay(30);
}

/**
 * @brief rutine, which sets colour based on accelerometer output
 */
void displayLISOutputRutine() {
	static int16_t accel_x = 0;
	static int16_t accel_y = 0;
	static int16_t accel_z = 0;
	//Update colour based on accelerometer output
	LIS3DH_getAxes(&accel_x, &accel_y, &accel_z);
	// 2. Convert to absolute values (so upside-down or backwards movement still creates light)
	// We divide by 4 to map ~1000 (1G of gravity) down to ~250 (perfect for an 8-bit LED channel)
	int16_t r_val = (accel_x >= 0 ? accel_x : (-accel_x)) / 64;
	int16_t g_val = (accel_y >= 0 ? accel_y : (-accel_y)) / 64;
	int16_t b_val = (accel_z >= 0 ? accel_z : (-accel_z)) / 64;

	// 3. Constrain the values between 0 and 255 just to protect against high-acceleration spikes
	if (r_val > 255)
		r_val = 255;
	if (g_val > 255)
		g_val = 255;
	if (b_val > 255)
		b_val = 255;
	if (r_val < b_val && r_val < g_val) {
		r_val = 0;
	} else if (b_val < g_val) {
		b_val = 0;
	} else {
		g_val = 0;
	}
	LP5817_setColor(SCALE((uint8_t )r_val), SCALE((uint8_t )g_val),
			SCALE((uint8_t )b_val));
	// Adjust this delay to speed up or slow down the transition.
	// 30ms gives a beautifully smooth, tranquil fade.
	HAL_Delay(30);
}

void updateMode() {
	if (measure) {
		int16_t x = 0, y = 0, z = 0;
		LIS3DH_getAxes(&x, &y, &z);

		if (change_mod) {
			//check z
			if (z < -10000) {
				mode++;
				mode %= 10;
			}
			change_mod = 0;
		} else {
			int32_t x_scaled = x / 64;
			int32_t y_scaled = y / 64;
			if ( (x_scaled * x_scaled) + (y_scaled * y_scaled) > 50000 )
			{
				change_mod = 1;
			}
		}
		measure = 0;
	}
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_I2C1_Init();
	/* USER CODE BEGIN 2 */

	uint8_t i2c_err = LP5817_init();
	HAL_Delay(1);
	handleI2CErr(i2c_err);

	LP5817_setColor(255, 0, 0);

	i2c_err = LIS3DH_init();
	HAL_Delay(1);
	handleI2CErr(i2c_err);

	HAL_Delay(500);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

		//get data from
		updateMode();

		uint8_t clear_buffer = 0;
		LIS3DH_read_reg(0x31, &clear_buffer, 1);

		switch (mode) {
		case 0:
			rainbowRutine();
			break;
		case 1:
			//red
			LP5817_setColor(SCALE(255), 0, 0);
			__WFI();
			break;
		case 2:
			//green
			LP5817_setColor(0, SCALE(255), 0);
			__WFI();
			break;
		case 3:
			//blue
			LP5817_setColor(0, 0, SCALE(255));
			__WFI();
			break;
		case 4:
			//magenta
			LP5817_setColor(SCALE(255), 0, SCALE(255));
			__WFI();
			break;
		case 5:
			//yellow
			LP5817_setColor(SCALE(255), SCALE(255), 0);
			__WFI();
			break;
		case 6:
			//cyan?
			LP5817_setColor(0, SCALE(255), SCALE(255));
			__WFI();
			break;
		case 7:
			//white
			LP5817_setColor(255, 255, 255);
			__WFI();
			break;
		case 8:
			//off
			LP5817_setColor(0, 0, 0);
			__WFI();
			break;
		case 9:
			displayLISOutputRutine();
		};

	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == LIS3_INT1_Pin) {
		measure = 1;
	}
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
