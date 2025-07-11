#include "Application.h"
#include <math.h>

// PI controller handle
PI_Handle_t PI_Handle;
float  Encoder_Angle;
float LinerDisp;
float PI_Out;
float PWM_Out;
uint8_t t;


MB_Status_t Modbus_CallBack(void *context) {
    MB_Slave_t *mb = (MB_Slave_t *)context;
    // ... service your request, fill mb->Response[], set ResponseLength ...
    return MB_STAT_OK;
}

//----------------------------------------------------------------------------//
//                             CAN-related definitions                        //
//----------------------------------------------------------------------------//

static void Config_CAN_Filters(void)
{
	const CAN_FilterTypeDef filter = {
			.FilterActivation      = CAN_FILTER_ENABLE,
			.FilterBank            = 10,
			.FilterFIFOAssignment  = CAN_FILTER_FIFO0,
			.FilterIdHigh          = 0x0000,
			.FilterIdLow           = 0x0000,
			.FilterMaskIdHigh      = 0x0000,
			.FilterMaskIdLow       = 0x0000,
			.FilterMode            = CAN_FILTERMODE_IDMASK,
			.FilterScale           = CAN_FILTERSCALE_32BIT,
	};

	HAL_CAN_ConfigFilter(&hcan1, &filter);
}

//----------------------------------------------------------------------------//
//                           Global Variable Declarations                      //
//----------------------------------------------------------------------------//
CAN_RxHeaderTypeDef RX_Header;
uint8_t             CAN_RxData[8];
uint8_t             TxData[TX_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0 };
CAN_TxHeaderTypeDef TxHeader;
uint32_t            TxMailbox = 0;
// values sent from feedback
int16_t             Motor_current =0,steering_wheel_angle =0,Steering_wheel_speed ,PWM_output=0 ;
uint32_t            Tmp = 0;
//values recieved from Steering unit
int16_t motor_current;
int16_t Rack_position;
int16_t Rack_force;
//int16_t PWM_Stee;

//----------------------------------------------------------------------------//
//                             Application Initialization                     //
//----------------------------------------------------------------------------//
void Application_Init(void)
{
	//-------------- Modbus initialization --------------//
	MB_Init_UART1(&huart1, 0x1);
	MB_Init_USB_MODBUS(&hUsbDeviceFS,0x2);


	MB.hw_interface.MB_Request_Recieved=&Modbus_CallBack;
	USB_MB.hw_interface.MB_Request_Recieved=&Modbus_CallBack;

	free(USB_MB.HoldingRegs);
	free(USB_MB.InputRegs);
	free(USB_MB.InputBits);
	free(USB_MB.CoilBits);

	USB_MB.HoldingRegs=MB.HoldingRegs;
	USB_MB.InputRegs=MB.InputRegs;
	USB_MB.InputBits=MB.InputBits;
	USB_MB.CoilBits=MB.CoilBits;

	//-------------- CAN initialization --------------//

	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
	Config_CAN_Filters();

	TxHeader.StdId = 0x101;       // Example standard ID
	TxHeader.ExtId = 0x01;
	TxHeader.IDE   = CAN_ID_STD;
	TxHeader.RTR   = CAN_RTR_DATA;
	TxHeader.DLC   = 8;           // 8-byte message


	//-------------- Encoder Timer (TIM4) initialization --------------//
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);

	//-------------- Motor timer (TIM8) initialization --------------//
	HAL_TIM_Base_Start(&htim8);
	Motor_Init(&htim8, TIM_CHANNEL_1);
	Motor_Init(&htim8, TIM_CHANNEL_2);

	//-------------- ADC trigger via TIM5 / DMA --------------//
	//HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
	HAL_TIM_Base_Start(&htim5);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2); //that triggers the ADC conversion
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *) Iregs->ADC_Raw_Values, 9);

	//LOAD default
	if (!Load_NV_Data()) {
		Load_Default_NV_Data();
	}

	//-------------- PI controller initialization --------------//
	PI_Init(
			&PI_Handle,
			DefaultParams.Control_Max_Out,
			DefaultParams.I_Control_Kp,
			DefaultParams.I_Control_Ki,
			DefaultParams.I_Control_Kc,
			DefaultParams.Controller_Sampling_Time
	);
	HAL_TIM_Base_Start_IT(&htim6); //to call the pi evaluat fun every 5 ms



	//--------------Interpolation initialization --------------//
	/*	//ENCODER VALUES
	M->output_max = 75.0f;
	M->output_min = -75.0f;
	//LINEAR VALUES
	M->input_max = 180.0f;
	M->input_min = -180.0f;*/
}

//----------------------------------------------------------------------------//
//                             Main Application Loop                          //
//----------------------------------------------------------------------------//
inline void Application_Run(void)
{

	uint32_t PID_Ticks = HAL_GetTick();


	while (1) {
		// Modbus routine
		MB_Slave_Routine(&MB, HAL_GetTick());

		MB_Slave_Routine(&USB_MB, HAL_GetTick());

		if(GetCoil(MB_Coil_TimerCNT_Reset))//if coil =1
		{
			__HAL_TIM_GET_COUNTER(&htim4)=0;
			SetCoil(MB_Coil_TimerCNT_Reset, 0);
		}

		//Calucate Encoder Angle
		Encoder_Angle= get_encoder_angle((int32_t)__HAL_TIM_GET_COUNTER(&htim4));

		LinerDisp =map_linear(M, Rack_position);

		// LED indication based on movement direction
		if (Encoder_Angle < 0) {
			// Clockwise movement → LEDs OFF
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
		} else if (Encoder_Angle > 0) {
			// Counter-clockwise movement → LEDs ON
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);
		}




		//ADC VALUES
		Compute_Analog_Measurements();

		//Motor Current
		Motor_current =Iregs->I_OUT;

		//Encoder_angle
		steering_wheel_angle =Encoder_Angle;


		Iregs->Encoder_Angle=Encoder_Angle;
		Iregs->Motor_EA_SP=LinerDisp  ;

t=1;
		// PI control update every 5 ms
		if (HAL_GetTick() >= PID_Ticks) {
			if (t) {
				PI_Out = PI_Eval(
						&PI_Handle,
						LinerDisp,
						Encoder_Angle
						// SP is the desired value from the interoplation
						// PV actual current actual angle
				);
				PWM_Out=PI_Out;
				Iregs->Motor_PWM_Out=PWM_Out;
				Iregs->Motor_Encoder_Error = PI_Handle.Error;

				if (PWM_Out > 0) {
					__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
					__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, (uint16_t )PWM_Out);

				} else {
					PWM_Out = fabsf(PWM_Out);
					__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 0);
					__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, (uint16_t) PWM_Out);

				}


			}

			else {
				__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 0);
				__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
			}
			PID_Ticks = HAL_GetTick() + 5;  // added 5ms
		}


		// Prepare and send CAN message
		PrepareCANMessage(TxData, Motor_current, steering_wheel_angle, Steering_wheel_speed, PWM_output);
		if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 3) {
			if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK) {
				Error_Handler();  // Handle error if sending fails
			}
		}



		if (GetCoil(MB_Coil_Update_Params)) {
			PI_Handle.KP     = Hregs->sParams.I_Control_Kp;
			PI_Handle.KI     = Hregs->sParams.I_Control_Ki;
			//SetCoil(MB_Coil_Update_Params, 0);
		}

		if (GetCoil(MB_Coil_Update_MAXOUT)) {
			PI_Handle.OutMax = Hregs->sParams.Control_Max_Out;
			SetCoil(MB_Coil_Update_MAXOUT, 0);
		}

		if (GetCoil(MB_Coil_Update_KC)) {
			PI_Handle.OutMax = Hregs->sParams.I_Control_Kc;
			SetCoil(MB_Coil_Update_KC, 0);
		}

		if (GetCoil(MB_Coil_Load_Defaults)) {
			Load_Default_NV_Data();
			SetCoil(MB_Coil_Load_Defaults, 0);
		}
	}
}

//----------------------------------------------------------------------------//
//                          ADC Measurement Computation                       //
//----------------------------------------------------------------------------//
void Compute_Analog_Measurements(void)
{
	//calculate the Vbus voltage
	Iregs->Vbus = ((float) Iregs->ADC_Raw_Values[1] * (DefaultParams.Vmotor_Sense_Gain))
                																														  - (DefaultParams.Vmotor_Sense_Offset);
	Iregs->I_OUT = ((float) Iregs->ADC_Raw_Values[0] * (DefaultParams.I_Sense_Gain)
			- (DefaultParams.I_Sense_Offset + DefaultParams.Amplifier_offset))
                																														   * 1000.0f;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	// Currently empty—add logic here if needed.
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan1)
{
	CAN_RxHeaderTypeDef RxHeader;
	uint8_t            RxData[8];
	char               uartBuffer[200];

	if (HAL_CAN_GetRxMessage(hcan1, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
		motor_current = Decode(RxData, 0, 11, 1);
		Rack_position = Decode(RxData, 11, 12, 1);
		Rack_force    = Decode(RxData, 23, 16, 1);
		//	pwm = Decode(RxData, 33, 1, 12, 1);

		sprintf(
				uartBuffer,
				"Received: motor_current=%d, Rack_position=%d, Rack_force=%d\n ",
				motor_current,
				Rack_position,
				+		Rack_force
		);
	}
	else {
		sprintf(uartBuffer, "Error receiving message!\n");
	}
	////	HAL_UART_Transmit(&huart1, (uint8_t *)uartBuffer, strlen(uartBuffer), 200);
	//CDC_Transmit_FS((uint8_t*) uartBuffer, strlen(uartBuffer));
}

