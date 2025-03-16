/*
 * HW_Interface.h
 *
 *  Created on: Mar 13, 2025
 *      Author: Ramy Sorial
 */

#ifndef INC_HW_INTERFACE_H_
#define INC_HW_INTERFACE_H_

#include "stm32f4xx_hal.h"

extern ADC_HandleTypeDef hadc1;

extern CAN_HandleTypeDef hcan1;

extern I2C_HandleTypeDef hi2c1;

extern SPI_HandleTypeDef hspi1;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim8;

extern UART_HandleTypeDef huart1;

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;


#endif /* INC_HW_INTERFACE_H_ */
