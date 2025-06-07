#ifndef HARDWARE_INTERFACE_USB_HARDWARE_INTERFACE_USB_H_
#define HARDWARE_INTERFACE_USB_HARDWARE_INTERFACE_USB_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "../../USB_DEVICE/App/usbd_cdc_if.h"

#include "../../Drivers/SbW_Protocol/SbW_protocol.h"

#define TxRxBufferSize 256

typedef struct{
	float Id;
	float Iq;
	float Vd;
	float Vq;
	float T;
	uint8_t K;
	uint16_t M;
	uint8_t G[8];
} DataFrame;

extern SbW_Protocol_t S;

// Transmit data to PC over USB CDC
void SbW_Protocol_Reply(uint8_t *data, uint16_t len);

//To notify the application layer
void App_User_Callback(SbW_Err_Codes_t Error_Code);

//Sampling timer callback
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif /* HARDWARE_INTERFACE_USB_HARDWARE_INTERFACE_USB_H_ */
