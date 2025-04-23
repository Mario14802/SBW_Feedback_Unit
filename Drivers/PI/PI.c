



#include "PI.h"

void PI_Init(PI_Handle_t *p, float MaxOut, float KP, float KI, float KC, float Ts)
{
    p->OutMax = MaxOut;  // Maximum allowed output (saturation limit)
    p->KP = KP;          // Proportional gain
    p->KI = KI;          // Integral gain
    p->KC = KC;          // Anti-windup gain (back-calculation factor)
    p->Ts = Ts;          // Sampling time
}

float PI_Eval(PI_Handle_t *p, float SetPoint, float PV)
{
    float Temp;
    // Compute error between desired setpoint and current process variable
    p->Error = SetPoint - PV;

    // Combine the proportional action and the accumulated error (integral)
    Temp = p->Error * p->KP + p->SumError;

    // Saturate the control output within the limits [-OutMax, OutMax]
    if (Temp > p->OutMax)
    {
        p->Out = p->OutMax;
    }
    else if (Temp < -p->OutMax)
    {
        p->Out = -p->OutMax;
    }
    else
    {
        p->Out = Temp;
    }

    // Compute the excess amount beyond saturation
    float Excess = Temp - p->Out;

    // Update the integral term with anti-windup correction
    p->SumError += (p->Error * p->KI * p->Ts) - (p->KC * Excess);

   /* p->KC * Excess subtracts from the integral accumulator.
    In effect, you “bleed off” some of the accumulated error whenever you’re saturated,
	preventing the integrator from winding up too far.*/

    // Return the final control output
    return p->Out;
}
