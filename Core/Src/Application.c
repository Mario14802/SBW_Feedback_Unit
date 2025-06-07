#include "Application.h"
#include <math.h>

// PI controller handle
PI_Handle_t PI_Handle;
float Encoder_Angle = 0;

//----------------------------------------------------------------------------//
//                             CAN-related definitions                        //
//----------------------------------------------------------------------------//

static void Config_CAN_Filters(void) {
	const CAN_FilterTypeDef filter = { .FilterActivation = CAN_FILTER_ENABLE,
			.FilterBank = 10, .FilterFIFOAssignment = CAN_RX_FIFO0,
			.FilterIdHigh = 0x0000, .FilterIdLow = 0x0000, .FilterMaskIdHigh =
					0x0000, .FilterMaskIdLow = 0x0000, .FilterMode =
			CAN_FILTERMODE_IDMASK, .FilterScale =
			CAN_FILTERSCALE_32BIT, };

	HAL_CAN_ConfigFilter(&hcan1, &filter);
}

//----------------------------------------------------------------------------//
//                           Global Variable Declarations                      //
//----------------------------------------------------------------------------//
CAN_RxHeaderTypeDef RX_Header;
uint8_t CAN_RxData[8];
uint8_t TxData[TX_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0 };
CAN_TxHeaderTypeDef TxHeader;
uint32_t TxMailbox = 0;
int16_t V1 = 0, V2 = 0, V3 = 0, V4 = 0;
uint32_t Tmp = 0;
//float temp = (float) Iregs->Vbus;
int16_t motor_current;
int16_t Rack_position;
int16_t Rack_force;
//----------------------USB_CDC_Config--------------------//
void Compute_Analog_Measurements(void);
DataFrame F;
uint8_t FrameBuffer[Def_Frame_Len * FIFO_Depth];
SbW_Protocol_t S = { .SamplingFreq = 15, .Frame_Len = Def_Frame_Len,
		/*.HW_Interface_t = { .Send_Reply = SbW_Protocol_Reply, .User_Callback =
				App_User_Callback },*/ .Fifo_Buffer = FrameBuffer,
		.Fifo_Buffer_Size = FIFO_Depth, .FrameDataBaseAddress = (uint8_t*) &F,
		.P_TXBuffer = TxBuffer, .RemainingFrames = 0 };

//----------------------------------------------------------------------------//
//                             Application Initialization                     //
//----------------------------------------------------------------------------//
void Application_Init(void) {
	//-------------- Modbus initialization --------------//
	MB_Init_UART1(&huart1, 0x1);

	//-------------- CAN initialization --------------//

	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
	Config_CAN_Filters();

	TxHeader.StdId = 0x101;       // Example standard ID
	TxHeader.ExtId = 0x01;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = 8;           // 8-byte message

	//-------------- Encoder Timer (TIM4) initialization --------------//
	HAL_TIM_Encoder_Start_IT(&htim4, TIM_CHANNEL_ALL);

	//-------------- Motor timer (TIM8) initialization --------------//
	HAL_TIM_Base_Start(&htim8);
	Motor_Init(&htim8, TIM_CHANNEL_1);
	Motor_Init(&htim8, TIM_CHANNEL_2);

	//-------------- ADC trigger via TIM5 / DMA --------------//
	//HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
	HAL_TIM_Base_Start(&htim5);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2); //that triggers the ADC conversion
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) Iregs->ADC_Raw_Values, 9);

	//LOAD default
	if (!Load_NV_Data()) {
		Load_Default_NV_Data();
	}

	//-------------- PI controller initialization --------------//
	PI_Init(&PI_Handle,
	MaxOut, DefaultParams.I_Control_Kp, DefaultParams.I_Control_Ki,
	KC, DefaultParams.Controller_Sampling_Time);
	HAL_TIM_Base_Start_IT(&htim6); //to call the pi evaluat fun every 5 ms

	//-----------------------USB_CDC_INIT--------------//
	SbW_Init(&S);
	HAL_TIM_Base_Start_IT(&htim1);
}

//----------------------------------------------------------------------------//
//                             Main Application Loop                          //
//----------------------------------------------------------------------------//
inline void Application_Run(void) {
	uint32_t Ticks = HAL_GetTick();
	uint32_t PID_Ticks = HAL_GetTick();
	//
	//HAL_StatusTypeDef S;

	while (1) {
		// Modbus routine
		MB_Slave_Routine(&MB, HAL_GetTick());

		// Prepare and send CAN message
		PrepareCANMessage(TxData, V1, V2, V3, V4);
		if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 3) {
			if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox)
					!= HAL_OK) {
				Error_Handler();  // Handle error if sending fails
			}
		}

		//HAL_Delay(100);
		//Encoder_Angle
		if (GetCoil(MB_Coil_TimerCNT_Reset))		//if coil =1
				{
			__HAL_TIM_GET_COUNTER(&htim4) = 0;
			SetCoil(MB_Coil_TimerCNT_Reset, 0);
		}

		Encoder_Angle = get_encoder_angle(
				(int32_t) __HAL_TIM_GET_COUNTER(&htim4));

		// LED indication based on movement direction
		if (Encoder_Angle < 0) {
			// Clockwise movement → LEDs OFF
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
		} else if (Encoder_Angle > 0) {
			// Counter-clockwise movement → LEDs ON
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_RESET);
		}

		//	HAL_Delay(5);

		//ADC VALUES
		Compute_Analog_Measurements();

		// PI control update every 5 ms
		if (HAL_GetTick() >= PID_Ticks) {
			if (GetCoil(MB_Coil_Enable_PI_Controller)) {
				Iregs->Motor_PWM_Out = PI_Eval(&PI_Handle, Hregs->Motor_I_SP, // sp is the desired value
						Iregs->I_OUT         // actual current
						);
				Iregs->Motor_I_Error = PI_Handle.Error;

				if (Iregs->Motor_PWM_Out > 0) {
					__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 0);
					__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1,
							(uint16_t ) Iregs->Motor_PWM_Out);
				} else {
					Iregs->Motor_PWM_Out = fabsf(Iregs->Motor_PWM_Out);
					__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
					//__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2,
					//        (uint16_t )PI_Control_Duty);
					//__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, (uint16_t )Iregs->Motor_PWM_Out);
				}
			} else {
				__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 0);
				__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
			}
			PID_Ticks = HAL_GetTick() + 5;  // added 5ms
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
		S.HW_Interface_t.Send_Reply = SbW_Protocol_Reply;
		S.HW_Interface_t.User_Callback = App_User_Callback;
		F.Id=Iregs->Vbus;
}
}

//----------------------------------------------------------------------------//
//                          ADC Measurement Computation                       //
//----------------------------------------------------------------------------//
void Compute_Analog_Measurements(void) {
//calculate the Vbus voltage
Iregs->Vbus = ((float) Iregs->ADC_Raw_Values[1]
		* (DefaultParams.Vmotor_Sense_Gain))
		- (DefaultParams.Vmotor_Sense_Offset);
Iregs->I_OUT = ((float) Iregs->ADC_Raw_Values[0] * (DefaultParams.I_Sense_Gain)
		- (DefaultParams.I_Sense_Offset + DefaultParams.Amplifier_offset))
		* 1000.0f;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
// Currently empty—add logic here if needed.
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan1) {
CAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[8];
char uartBuffer[200];

if (HAL_CAN_GetRxMessage(hcan1, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
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
