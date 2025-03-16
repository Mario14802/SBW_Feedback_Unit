/*
 * Motor_Driver.c
 *
 *  Created on: Mar 14, 2025
 *      Author: Ramy Sorial
 */


#include "../Inc/Motor_Driver.h"
TIM_HandleTypeDef *Motor_Timer;

void Motor_Init(TIM_HandleTypeDef *Motor_Timer,uint8_t direction,uint16_t value)
{
HAL_TIM_PWM_Start(Motor_Timer, TIM_CHANNEL_1);
HAL_TIMEx_PWMN_Start(Motor_Timer, TIM_CHANNEL_1);

HAL_TIM_PWM_Start(Motor_Timer, TIM_CHANNEL_2);
HAL_TIMEx_PWMN_Start(Motor_Timer, TIM_CHANNEL_2);



}
void Motor_Pwm_Duty(TIM_HandleTypeDef*Motor_Timer,uint32_t DutyA,uint32_t DutyB)
{
 DutyA=(DutyA )%2799;
 DutyB=(DutyB )%2799;



 __HAL_TIM_SET_COMPARE(Motor_Timer,TIM_CHANNEL_1,DutyA);
 __HAL_TIM_SET_COMPARE(Motor_Timer,TIM_CHANNEL_1,DutyB);

}
