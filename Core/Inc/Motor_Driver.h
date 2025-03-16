/*
 * Motor_Driver.h
 *
 *  Created on: Mar 14, 2025
 *      Author: Ramy Sorial
 */

#ifndef INC_MOTOR_DRIVER_H_
#define INC_MOTOR_DRIVER_H_

#include "main.h"
#include "stm32f4xx_hal.h"

#include <stdint.h>

extern TIM_HandleTypeDef *Motor_Timer;

void Motor_Init(TIM_HandleTypeDef *Motor_Timer,uint8_t direction,uint16_t value);
void Motor_Pwm_Duty(TIM_HandleTypeDef*Motor_Timer,uint32_t DutyA,uint32_t DutyB);



#endif /* INC_MOTOR_DRIVER_H_ */
