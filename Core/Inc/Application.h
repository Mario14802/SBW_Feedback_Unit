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

extern ADC_HandleTypeDef hadc1;

extern CAN_HandleTypeDef hcan1;

extern I2C_HandleTypeDef hi2c1;

extern SPI_HandleTypeDef hspi1;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim8;

extern UART_HandleTypeDef huart1;

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

extern const SystemParams_t DefaultParams;

extern PI_Handle_t PI_Handle;

#define KC 0.9
#define MaxOut 500 //Maximum allowed output (saturation limit)

//modbus slave addreese
#define SLA 0x1

extern MB_Slave_t MB;

void Application_Init();

void Application_Run();
void Compute_Analog_Measurements();
#endif /* INC_APPLICATION_H_ */
