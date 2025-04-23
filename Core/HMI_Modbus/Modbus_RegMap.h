#ifndef Modbus_RegMap_h
#define Modbus_RegMap_h

#include "../../Drivers/Modbus_RTU_C/Modbus_Slave.h"
#include "../HMI_Modbus/NV_Params.h"

extern MB_Slave_t MB;

typedef struct HoldingRegs
{
    //holding registers will be divided into 2 parts

    //assume that the number of holding registers reserved for the 
    //system params is 128 words
    //part 1 ... is the system parameters
    // 0-127
    SystemParams_t sParams;
    uint16_t Dummy_1[128-(sizeof(SystemParams_t)/2)];


    //part 2 is the system states that need to be read/write
    float Motor_I_SP;
}HoldingRegs_t;
//////////////////////
typedef struct InputRegs
{
    //define all the system states (which are read-only)

    float I_OUT;//0,1
    float Vbus;//2,3
    float Torque;//4,5

    float Motor_PWM_Out;//6,7
    float Motor_I_Error;//8,9

    uint16_t ADC_Raw_Values[12];
}InputRegs_t;
//////////////////
enum InputBits // 0 till 15
{
    //DRV8301 FAULTS 
    MB_Input_PI_Enabled,
};

//////////////////
enum CoilBits // 0 till 15
{
	MB_Coil_Enable_System,
	MB_Coil_Enable_PI_Controller,
	MB_Coil_Update_Params,
	MB_Coil_Load_Defaults,

};
#define GetInputBit(bit) MB_Parse_Bit(MB.InputBits, bit)
#define SetInputBit(bit, State) MB_Encode_Bit(MB.InputBits, bit, State)

#define GetCoil(bit) MB_Parse_Bit(MB.CoilBits, bit)
#define SetCoil(bit, State) MB_Encode_Bit(MB.CoilBits, bit, State)

#define HoldingRegsSize (sizeof(HoldingRegs_t)/2)
#define InputRegsSize (sizeof(InputRegs_t)/2)

#endif
