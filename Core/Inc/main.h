/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_13
#define LED1_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_14
#define LED2_GPIO_Port GPIOC
#define LED3_Pin GPIO_PIN_15
#define LED3_GPIO_Port GPIOC
#define AUX7_Pin GPIO_PIN_2
#define AUX7_GPIO_Port GPIOC
#define AUX8_Pin GPIO_PIN_3
#define AUX8_GPIO_Port GPIOC
#define AUX9_Pin GPIO_PIN_4
#define AUX9_GPIO_Port GPIOC
#define AUX10_Pin GPIO_PIN_5
#define AUX10_GPIO_Port GPIOC
#define AUX1_Pin GPIO_PIN_2
#define AUX1_GPIO_Port GPIOB
#define AUX2_Pin GPIO_PIN_10
#define AUX2_GPIO_Port GPIOB
#define AUX3_Pin GPIO_PIN_11
#define AUX3_GPIO_Port GPIOB
#define AUX4_Pin GPIO_PIN_12
#define AUX4_GPIO_Port GPIOB
#define AUX5_Pin GPIO_PIN_13
#define AUX5_GPIO_Port GPIOB
#define AUX6_Pin GPIO_PIN_15
#define AUX6_GPIO_Port GPIOB
#define AUX11_Pin GPIO_PIN_8
#define AUX11_GPIO_Port GPIOC
#define AUX12_Pin GPIO_PIN_9
#define AUX12_GPIO_Port GPIOC
#define AUX0_Pin GPIO_PIN_15
#define AUX0_GPIO_Port GPIOA
#define AUX13_Pin GPIO_PIN_10
#define AUX13_GPIO_Port GPIOC
#define SPI1_SS3_Pin GPIO_PIN_11
#define SPI1_SS3_GPIO_Port GPIOC
#define SPI1_SS2_Pin GPIO_PIN_12
#define SPI1_SS2_GPIO_Port GPIOC
#define SPI1_SS1_Pin GPIO_PIN_2
#define SPI1_SS1_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
