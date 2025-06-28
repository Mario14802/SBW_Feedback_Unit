/*
 * HMI_Modbus.h
 *
 *  Created on: Jan 23, 2025
 *      Author: MARIO
 */

#ifndef USB_MODBUS_H_
#define USB_MODBUS_H_



#include "main.h"
#include "stm32f4xx_hal.h"

#include <stdint.h>
#include "../../Drivers/Modbus_RTU_C/Modbus_Slave.h"
#include "../HMI_Modbus/Modbus_RegMap.h"
#include "../App/usbd_cdc_if.h"


extern MB_Slave_t USB_MB;
extern USBD_CDC_HandleTypeDef * UsbCdc;

extern USBD_CDC_HandleTypeDef  UsbCdc1;

extern HoldingRegs_t *Hregs; //test
extern InputRegs_t *Iregs;

//MB_Status_t Init_HMI(UART_HandleTypeDef *uart, uint8_t SLA);
MB_Status_t MB_Init_USB_MODBUS(USBD_CDC_HandleTypeDef *usb_handle, uint8_t slave_address);




#endif /* USB_MODBUS_H_ */
