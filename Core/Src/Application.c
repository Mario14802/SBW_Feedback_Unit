#include "Application.h"
#include <math.h>

//TIM_HandleTypeDef *Motor_Timer = htim5;
PI_Handle_t PI_Handle;

extern TIM_HandleTypeDef htim6;

static void Config_CAN_Filters() {
	CAN_FilterTypeDef filter = { .FilterActivation = CAN_FILTER_ENABLE,
			.FilterBank = 10, .FilterFIFOAssignment = CAN_RX_FIFO0,
			.FilterIdHigh = 0x0000, .FilterIdLow = 0x0000,
			.FilterMaskIdHigh = 0, .FilterMaskIdLow = 0x0000, .FilterMode =
			CAN_FILTERMODE_IDMASK, .FilterScale =
			CAN_FILTERSCALE_32BIT, };
	HAL_CAN_ConfigFilter(&hcan1, &filter);

}

CAN_RxHeaderTypeDef RX_Header;
uint8_t CAN_RxData[8];
uint8_t TxData[TX_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0 };
CAN_TxHeaderTypeDef TxHeader;
uint32_t TxMailbox = 0;
int16_t V1 = 0, V2 = 0, V3 = 0, V4 = 0;

uint32_t Tmp = 0;

int16_t motor_current;
int16_t Rack_position;
int16_t Rack_force;

void Application_Init() {
	//CAN init
	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
	Config_CAN_Filters();
	TxHeader.StdId = 0x101;  // Example standard ID  ********
	TxHeader.ExtId = 0x01;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = 8;  // 8-byte message
	//modbus init
	MB_Init_UART1(&huart1, SLA);
	HAL_TIM_Base_Start(&htim8);
	Motor_Init(&htim8, TIM_CHANNEL_1);
	Motor_Init(&htim8, TIM_CHANNEL_2);
	//HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
	HAL_TIM_Base_Start(&htim5);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2); //that triggers the ADC conversion
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) Iregs->ADC_Raw_Values, 9);

	if (!Load_NV_Data()) {
		Load_Default_NV_Data();
	}

	PI_Init(&PI_Handle, MaxOut, DefaultParams.I_Control_Kp,
			DefaultParams.I_Control_Ki, KC,
			DefaultParams.Controller_Sampling_Time);
	HAL_TIM_Base_Start_IT(&htim6); //to call the pi evaluat fun every 5 ms

}

inline void Application_Run() {
	uint32_t Ticks = HAL_GetTick();
	uint32_t PID_Ticks = HAL_GetTick();
	HAL_StatusTypeDef S;
	while (1) {
		//Modbus routine
		MB_Slave_Routine(&MB, HAL_GetTick());

		PrepareCANMessage(TxData, V1, V2, V3, V4);
		if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 3) {
			if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox)
					!= HAL_OK) {
				Error_Handler();  // Handle error if sending fails
			}
		}
		HAL_Delay(100);
		/*if (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0) {
		 S = HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RX_Header,
		 CAN_RxData);
		 S = HAL_CAN_AddTxMessage(&hcan1, &TxHeader, CAN_RxData, &TxMailbox);
		 //toggle the LED gpio upon message reception
		 HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		 }
		 if (HAL_GetTick() >= Ticks) {
		 if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 3) {
		 HAL_CAN_AddTxMessage(&hcan1, &TxHeader, CAN_RxData, &TxMailbox);
		 }

		 Ticks = HAL_GetTick() + 250;
		 //HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		 HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
		 //Motor_Pwm_Duty(&htim5,0,0);
		 //Motor_Pwm_Duty(&htim5, TIM_CHANNEL_1, TIM_CHANNEL_2, 500, TIMER8_ARR, CLOCKWISE);

		 Compute_Analog_Measurements();

		 if(PI_Control_Duty <0)
		 {
		 Motor_Pwm_Duty(Motor_Timer, TIM_CHANNEL_1, TIM_CHANNEL_2, (uint32_t) PI_Control_Duty, TIMER5_ARR, CLOCKWISE);
		 }

		 else
		 {
		 Motor_Pwm_Duty(Motor_Timer, TIM_CHANNEL_1, TIM_CHANNEL_2, (uint32_t) PI_Control_Duty, TIMER5_ARR, ANTI_CLOCKWISE);

		 }

		 //memcpy(&Iregs->ADC_Raw_Values,ADC_BUFFER,sizeof(ADC_BUFFER));
		 // Tmp = ADC_Buffer[1];
		 // Tmp *= 26435;
		 //	 MB.InputRegs[10] = (uint16_t)(Tmp>>13);
		 }*/
		if (HAL_GetTick() > PID_Ticks) {
			if (GetCoil(MB_Coil_Enable_PI_Controller)) {
				Iregs->Motor_PWM_Out = PI_Eval(&PI_Handle, Hregs->Motor_I_SP, //sp is the desired value
						Iregs->I_OUT); //  PI_Eval called here every 5ms
				Iregs->Motor_I_Error = PI_Handle.Error;
				if (Iregs->Motor_PWM_Out > 0) {
					__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 0);
					__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1,
							(uint16_t )Iregs->Motor_PWM_Out);

				} else {
					Iregs->Motor_PWM_Out = fabsf(Iregs->Motor_PWM_Out);
					__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
					//__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2,
					//		(uint16_t )PI_Control_Duty);
					//__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, (uint16_t )Iregs->Motor_PWM_Out);
				}
			} else {
				__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 0);
				__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
			}
			PID_Ticks = HAL_GetTick() + 5;					//added 5ms
		}

		if (GetCoil(MB_Coil_Update_Params)) {
			PI_Handle.KP = Hregs->sParams.I_Control_Kp;
			PI_Handle.KI = Hregs->sParams.I_Control_Ki;
			PI_Handle.OutMax = Hregs->sParams.I_Control_Max_Out;
			SetCoil(MB_Coil_Update_Params, 0);
		}
		if (GetCoil(MB_Coil_Load_Defaults)) {
			Load_Default_NV_Data();
			SetCoil(MB_Coil_Load_Defaults, 0);
		}

	}
}

void Compute_Analog_Measurements() {
	//calculate the Vbus voltage
	Iregs->Vbus = ((float) Iregs->ADC_Raw_Values[1]
			* (DefaultParams.Vmotor_Sense_Gain))
			- (DefaultParams.Vmotor_Sense_Offset);
	Iregs->I_OUT = ((float) Iregs->ADC_Raw_Values[0]
			* (DefaultParams.I_Sense_Gain)
			- (DefaultParams.I_Sense_Offset + DefaultParams.Amplifier_offset))
			* 1000.0f;

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {

}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan1) {
	CAN_RxHeaderTypeDef RxHeader;
	uint8_t RxData[8];
	char uartBuffer[200];

	if (HAL_CAN_GetRxMessage(hcan1, CAN_RX_FIFO0, &RxHeader, RxData)
			== HAL_OK) {

		motor_current = Decode(RxData, 0, 11, 1);
		Rack_position = Decode(RxData, 12, 12, 1);
		Rack_force = Decode(RxData, 23, 16, 1);
		//int16_t pwm = Decode(RxData, 33, 1, 12, 1);

		sprintf(uartBuffer,
				"Received: motor_current=%d, Rack_position=%d, Rack_force=%d\n ",
				motor_current, Rack_position, Rack_force);
	} else {
		sprintf(uartBuffer, "Error receiving message!\n");
	}
	HAL_UART_Transmit(&huart1, (uint8_t*) uartBuffer, strlen(uartBuffer), 200);
	//CDC_Transmit_FS((uint8_t*) uartBuffer, strlen(uartBuffer));
}

