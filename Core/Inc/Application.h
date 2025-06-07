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
#include "usb_device.h"
//#include "HW_Interface.h"
#include "../../Drivers/EEPROM_Flash/EEPROM_DRIVER.h"
#include "../../Drivers/Motor_Driver/Motor_Driver.h"
#include "../HMI_Modbus/HMI_Modbus.h"
#include "../Inc/Modbus_EEPROM_SL.h"
#include "../../Drivers/PI/PI.h"
#include "../../Drivers/Incremntal Encoder/Encoder.h"
#include "Feedback_CAN.h"
#include "../../Drivers/SbW_Protocol/SBW_protocol.h"
#include "../Hardware_Interface_USB/Hardware_Interface_USB.h"

extern ADC_HandleTypeDef hadc1;
extern CAN_FilterTypeDef canFilterConfig;

extern CAN_HandleTypeDef hcan1;

//extern I2C_HandleTypeDef hi2c1;

extern SPI_HandleTypeDef hspi1;

extern TIM_HandleTypeDef htim1;//USB_CDC
extern TIM_HandleTypeDef htim4;//Encoder Timer
extern TIM_HandleTypeDef htim5;//Trigger ADC
extern TIM_HandleTypeDef htim6;//PI evaluate
extern TIM_HandleTypeDef htim8;//Motor Timer

extern UART_HandleTypeDef huart1;

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

extern const SystemParams_t DefaultParams;

extern PI_Handle_t PI_Handle;


#define KC 0.9
#define MaxOut 1500 //Maximum allowed output (saturation limit)

//modbus slave addreese
#define SLA 0x1

extern MB_Slave_t MB;

extern uint8_t TxBuffer[];
extern uint8_t RxBuffer[];

#define Def_Frame_Len sizeof(DataFrame)
#define FIFO_Depth 50

void Application_Init();

void Application_Run();
void Compute_Analog_Measurements();
//void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan1);
#endif /* INC_APPLICATION_H_ */
