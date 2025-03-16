#include "Application.h"
//MB_Slave_t MB;

//TIM_HandleTypeDef *Motor_Timer = htim5;

static void Config_CAN_Filters()
{
	CAN_FilterTypeDef filter =
	{ .FilterActivation = ENABLE, .FilterBank = 0, .FilterFIFOAssignment = CAN_FILTER_FIFO0, .FilterIdHigh = 0, .FilterIdLow = 0, .FilterMaskIdHigh = 0, .FilterMaskIdLow = 0, .FilterMode = CAN_FILTERMODE_IDMASK, .FilterScale = CAN_FILTERSCALE_32BIT, .SlaveStartFilterBank = 14, };
	HAL_CAN_ConfigFilter(&hcan1, &filter);
	HAL_CAN_Start(&hcan1);
}

CAN_TxHeaderTypeDef H =
{ .DLC = 1, .StdId = 0x123, .IDE = CAN_ID_STD, .RTR = CAN_RTR_DATA };

void Application_Init()
{
	Config_CAN_Filters();
	//modbus init
	MB_Init_UART1(&huart1,SLA);

}

CAN_RxHeaderTypeDef RX_Header;
uint8_t CAN_RxData[8];
CAN_TxHeaderTypeDef TxHeader;
uint32_t TxMailbox = 0;

inline void Application_Run()
{
	uint32_t Ticks = HAL_GetTick();
	HAL_StatusTypeDef S;


	while (1)
	{
		//Modbus routine
		MB_Slave_Routine(&MB,HAL_GetTick());

		if (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0)
		{
			S = HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RX_Header, CAN_RxData);
			S = HAL_CAN_AddTxMessage(&hcan1, &TxHeader, CAN_RxData, &TxMailbox);
			//toggle the LED gpio upon message reception
			HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		}
		if (HAL_GetTick() >= Ticks)
		{
			if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 3)
			{
				HAL_CAN_AddTxMessage(&hcan1, &TxHeader, CAN_RxData, &TxMailbox);
			}

			Ticks = HAL_GetTick() + 250;
			//HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
			HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
		}
	}
}
