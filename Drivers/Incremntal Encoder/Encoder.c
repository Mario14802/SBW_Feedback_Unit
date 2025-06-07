/*
 * Encoder.c
 *
 *  Created on: Jun 5, 2025
 *      Author: MARIO
 */


#include "Encoder.h"

float angle_accum = 0.0f;
int32_t prev_count = 0;

float get_encoder_angle(int32_t Timer_counter)
{
    static float angle_accum = 0.0f;    // lives for the entire program
    static int32_t prev_count  = 0;

    int32_t encoder_count = Timer_counter;
    int32_t delta = encoder_count - prev_count;
    if      (delta >  (PPR/2)) delta -= PPR;
    else if (delta < -(PPR/2)) delta += PPR;

    // accumulate
    angle_accum += (float)delta * ANGLE_PER_COUNT;

    // clamp if you really need to cap at ±180°
    if      (angle_accum >  180.0f) angle_accum =  180.0f;
    else if (angle_accum < -180.0f) angle_accum = -180.0f;

    prev_count = encoder_count;
    return angle_accum;
}
