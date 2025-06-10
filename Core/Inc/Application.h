/*
 * Application.h
 *
 *  Created on: Mar 13, 2025
 *      Author: Ramy Sorial
 */

#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

#include "stm32f4xx_hal.h"
#include "main.h"
//#include "HW_Interface.h"
#include "../../Drivers/EEPROM_Flash/EEPROM_DRIVER.h"
#include "../../Drivers/Motor_Driver/Motor_Driver.h"
#include "../HMI_Modbus/HMI_Modbus.h"
#include "../Inc/Modbus_EEPROM_SL.h"
#include "../../Drivers/PI/PI.h"
#include "../../Drivers/Incremntal Encoder/Encoder.h"
#include "../../Drivers/Interpolation/Interpolation.h"
#include "Feedback_CAN.h"

extern ADC_HandleTypeDef hadc1;
extern CAN_FilterTypeDef canFilterConfig;

extern CAN_HandleTypeDef hcan1;

//extern I2C_HandleTypeDef hi2c1;

extern SPI_HandleTypeDef hspi1;

extern TIM_HandleTypeDef htim1;//
extern TIM_HandleTypeDef htim4; //encoder timer
extern TIM_HandleTypeDef htim5;//trigger adc
extern TIM_HandleTypeDef htim6;//pi evaluate
extern TIM_HandleTypeDef htim8;//motor timer

extern UART_HandleTypeDef huart1;

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

extern const SystemParams_t DefaultParams;

extern PI_Handle_t PI_Handle;

//interpolation
extern mapping_t M;

#define KC 0.9
#define MaxOut 1400 //Maximum allowed output (saturation limit)

//modbus slave addreese
#define SLA 0x1

extern MB_Slave_t MB;

void Application_Init();

void Application_Run();
void Compute_Analog_Measurements();
//void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan1);
#endif /* INC_APPLICATION_H_ */
