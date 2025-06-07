#include "Hardware_Interface_USB.h"

//Communication buffers
uint8_t TxBuffer[TxRxBufferSize];
uint8_t RxBuffer[TxRxBufferSize];

extern DataFrame F;

void SbW_Protocol_Reply(uint8_t *data, uint16_t len) {
	CDC_Transmit_FS(data, len);
}

void App_User_Callback(SbW_Err_Codes_t Error_Code) {
	// Implement error handling if needed
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	//F.G[0]++;
	SbW_Timer_Callback(&S);
}
