#ifndef _NV_Params_c
#define NV_Params_C

#include <stdint.h>

typedef struct
{
    //PID controller gains
    float I_Control_Kp; //0,1
    float I_Control_Ki; //2,3
    float I_Filter_Fc; //4,5
    float Controller_Sampling_Time; //6,7

    float I_Control_Max_Out;//8,9


    //Current sensor filter constant

    //current sensor params for phase A and B
    float I_Sense_Gain; //12,13
    float I_Sense_Offset; //14,15
    float Vmotor_Sense_Gain; //16,17
    float Vmotor_Sense_Offset; //18,19

    //differential angle torque sensor
    float Angle_Sensor_1_Gain;
    float Angle_Sensor_1_Offset;
    float Angle_Sensor_2_Gain;
    float Angle_Sensor_2_Offset;
    float Torque_Signal_Gain;
    float Torque_Signal_offset;
    float Angle_Sensor_Fc; //LPF cutoff frequency
    float  Amplifier_offset;//amplifier offset


    uint16_t Checksum; //38,39

}SystemParams_t;

#define SystemParams_Size_Words (sizeof(SystemParams_t)/2)





#endif
