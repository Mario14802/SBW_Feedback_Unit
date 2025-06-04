#include "../Inc/Modbus_EEPROM_SL.h"

const SystemParams_t DefaultParams = {
		.Angle_Sensor_1_Offset = 0,
		.Angle_Sensor_2_Offset = 0,
		.Torque_Signal_offset = 0,
		.I_Sense_Offset =0.0002015,
		.Vmotor_Sense_Offset = 0,
		.I_Filter_Fc = 50,
		.Angle_Sensor_Fc = 50,
		.Angle_Sensor_1_Gain = 1,
		.Angle_Sensor_2_Gain =1,
		.Torque_Signal_Gain = 1,
		.I_Sense_Gain = 0.005372f,
		.Vmotor_Sense_Gain = 0.00322f,
		.Amplifier_offset=0.021,
		.I_Control_Kp = 0.15,
		.I_Control_Ki = 0.8,
		.Controller_Sampling_Time = 1.0f/200.0f,
		.Checksum = 0,
		.I_Control_Max_Out = 500};

static uint16_t Compute_Checksum(uint8_t *Data, uint8_t Len) {
	uint16_t Chk = 0;
	for (uint16_t x = 0; x < Len - 4; x++) {
		Chk += Data[x];
	}
	return Chk;
}

uint8_t Load_NV_Data() {
	EEPROM_Process_Area(&HWIN, 0, (uint8_t*) &Hregs->sParams, Size, Read);
	if (Compute_Checksum((uint8_t*) &Hregs->sParams, Size)
			== Hregs->sParams.Checksum) {
		return 1;
	} else {
		return 0;
	}
}

uint8_t Load_Default_NV_Data() {
	memcpy(&Hregs->sParams, &DefaultParams, sizeof(DefaultParams));
	return Save_NV_Data();
}

uint8_t Save_NV_Data() {
	Hregs->sParams.Checksum = Compute_Checksum((uint8_t*) &Hregs->sParams,
			Size);
	EEPROM_Process_Area(&HWIN, 0, (uint8_t*) &Hregs->sParams, Size, Write);
	return 1;
}

