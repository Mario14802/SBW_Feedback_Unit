/*
 * PI.h
 *
 *  Created on: Mar 3, 2025
 *      Author: MARIO
 */

#ifndef MOTOR_CONTROL_PI_PI_H_
#define MOTOR_CONTROL_PI_PI_H_

typedef struct
{
	float Error;
	float SumError;

	float Out;
	float OutMax;

	//Controller Constants
	float KP;
	float KI;
	float KC; //Integral Attenuation Factor
	float Ts; //Controller Sampling Time

}PI_Handle_t;

void PI_Init(PI_Handle_t *p, float MaxOut, float KP, float KI, float KC, float Ts);
float PI_Eval(PI_Handle_t *p, float SetPoint, float PV);





#endif /* MOTOR_CONTROL_PI_PI_H_ */
