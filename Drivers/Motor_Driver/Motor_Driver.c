/*
 * Motor_Driver.c
 *
 *  Created on: Mar 14, 2025
 *      Author: Ramy Sorial
 */


#include "Motor_Driver.h"
TIM_HandleTypeDef *Motor_Timer;

void Motor_Init(TIM_HandleTypeDef *Motor_Timer,uint32_t timer_channel)
{
	HAL_TIM_PWM_Start(Motor_Timer, timer_channel);
	HAL_TIMEx_PWMN_Start(Motor_Timer, timer_channel);

}
void Motor_Pwm_Duty(TIM_HandleTypeDef* Motor_Timer, uint32_t Tim_Channel1, uint32_t Tim_Channel2, uint32_t PI_Duty, uint16_t ARR_Value, uint32_t Direction)
{
    // Validate the input parameters
    if (Motor_Timer == NULL) {
        // Timer handle is invalid, return early
        return;
    }

    // Ensure ARR_Value is not zero to prevent division by zero or invalid duty
    if (ARR_Value == 0) {
        // Invalid ARR value, return early
        return;
    }

    // Ensure Duty is within the valid range (0 to ARR_Value - 1)
    PI_Duty = PI_Duty % ARR_Value;

    // Check for valid direction
    if (Direction != CLOCKWISE && Direction != ANTI_CLOCKWISE) {
        // Invalid direction, return early
        return;
    }

    // Set the PWM duty cycle based on the direction
    switch (Direction)
    {
    case CLOCKWISE:
        // For clockwise, set Tim_Channel1 with the Duty and Tim_Channel2 to 0
        __HAL_TIM_SET_COMPARE(Motor_Timer, Tim_Channel1, PI_Duty);
        __HAL_TIM_SET_COMPARE(Motor_Timer, Tim_Channel2, 0);
        break;

    case ANTI_CLOCKWISE:
        // For anticlockwise, set Tim_Channel1 to 0 and Tim_Channel2 with the Duty
        __HAL_TIM_SET_COMPARE(Motor_Timer, Tim_Channel1, 0);
        __HAL_TIM_SET_COMPARE(Motor_Timer, Tim_Channel2, PI_Duty);
        break;

    default:
        // This case should never occur due to earlier validation, but included for completeness
        break;
    }
}

void Timer_test(TIM_HandleTypeDef*Motor_Timer,uint32_t timer_channel ,uint32_t Duty,uint16_t ARR_Value)
{

	Duty=(Duty)%ARR_Value;
	__HAL_TIM_SET_COMPARE(Motor_Timer,timer_channel,Duty);




}
