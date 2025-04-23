/*
 * Motor_Driver.h
 *
 *  Created on: Mar 14, 2025
 *      Author: Ramy Sorial
 */

#ifndef MOTOR_DRIVER_MOTOR_DRIVER_H_
#define MOTOR_DRIVER_MOTOR_DRIVER_H_

#include "main.h"
#include "stm32f4xx_hal.h"

#include <stdint.h>

extern TIM_HandleTypeDef *Motor_Timer;

#define CLOCKWISE 0
#define ANTI_CLOCKWISE 1




void Motor_Init(TIM_HandleTypeDef *Motor_Timer,uint32_t timer_channel);
void Motor_Pwm_Duty(TIM_HandleTypeDef*Motor_Timer,uint32_t Tim_Channel1,uint32_t Tim_Channel2 ,uint32_t PI_Duty,uint16_t ARR_Value,uint32_t Direction);
void Timer_test(TIM_HandleTypeDef*Motor_Timer,uint32_t timer_channel ,uint32_t Duty,uint16_t ARR_Value);


#endif /* MOTOR_DRIVER_MOTOR_DRIVER_H_ */
