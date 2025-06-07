/*
 * Interpolation.h
 *
 *  Created on: Jun 7, 2025
 *      Author: MARIO
 */

#ifndef INTERPOLATION_INTERPOLATION_H_
#define INTERPOLATION_INTERPOLATION_H_



#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct
{
    float output_max; // e.g. 	 180 (units of output)
    float output_min; // e.g.   -180.0f (units of output)
    float input_max;  // e.g.    200.0f (units of input)
    float input_min;  // e.g.    0.0f (units of input)
} mapping_t;

extern mapping_t M;

float map_linear( mapping_t M, float in_value);

#endif /* INTERPOLATION_INTERPOLATION_H_ */
