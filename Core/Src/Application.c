#include "Application.h"
#include <math.h>

//TIM_HandleTypeDef *Motor_Timer = htim5;
PI_Handle_t PI_Handle;

extern TIM_HandleTypeDef htim6;

static void Config_CAN_Filters() {
	CAN_FilterTypeDef filter = { .FilterActivation = ENABLE, .FilterBank = 0,
			.FilterFIFOAssignment = CAN_FILTER_FIFO0, .FilterIdHigh = 0,
			.FilterIdLow = 0, .FilterMaskIdHigh = 0, .FilterMaskIdLow = 0,
			.FilterMode = CAN_FILTERMODE_IDMASK, .FilterScale =
			CAN_FILTERSCALE_32BIT, .SlaveStartFilterBank = 14, };
	HAL_CAN_ConfigFilter(&hcan1, &filter);
	HAL_CAN_Start(&hcan1);
}

CAN_TxHeaderTypeDef H = { .DLC = 1, .StdId = 0x123, .IDE = CAN_ID_STD, .RTR =
CAN_RTR_DATA };

void Application_Init() {
	Config_CAN_Filters();
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
			DefaultParams.I_Control_Ki, KC, DefaultParams.Controller_Sampling_Time);
	HAL_TIM_Base_Start_IT(&htim6);//to call the pi evaluat fun every 5 ms
}

CAN_RxHeaderTypeDef RX_Header;
uint8_t CAN_RxData[8];
CAN_TxHeaderTypeDef TxHeader;
uint32_t TxMailbox = 0;

uint32_t Tmp = 0;

inline void Application_Run() {
	uint32_t Ticks = HAL_GetTick();
	uint32_t PID_Ticks = HAL_GetTick();
	HAL_StatusTypeDef S;
	while (1) {
		//Modbus routine
		MB_Slave_Routine(&MB, HAL_GetTick());

		if (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0) {
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

			/*			if(PI_Control_Duty <0)
			 {
			 Motor_Pwm_Duty(Motor_Timer, TIM_CHANNEL_1, TIM_CHANNEL_2, (uint32_t) PI_Control_Duty, TIMER5_ARR, CLOCKWISE);
			 }

			 else
			 {
			 Motor_Pwm_Duty(Motor_Timer, TIM_CHANNEL_1, TIM_CHANNEL_2, (uint32_t) PI_Control_Duty, TIMER5_ARR, ANTI_CLOCKWISE);

			 }*/

			//memcpy(&Iregs->ADC_Raw_Values,ADC_BUFFER,sizeof(ADC_BUFFER));
			// Tmp = ADC_Buffer[1];
			// Tmp *= 26435;
			//	 MB.InputRegs[10] = (uint16_t)(Tmp>>13);
		}
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
					__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, (uint16_t )Iregs->Motor_PWM_Out);
				}
			} else {
				__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 0);
				__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
			}
			PID_Ticks = HAL_GetTick() + 5;//added 5ms
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

