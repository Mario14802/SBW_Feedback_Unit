/*
 * Interpolation.c
 *
 *  Created on: Jun 7, 2025
 *      Author: MARIO
 */


#include "Interpolation.h"
/**
 * map_linear():
 *   Given a mapping_t M (with float endpoints) and an input value in_value,
 *   compute the interpolated output value (float) between [M.output_min .. M.output_max].
 *
 *   Formula (all in float):
 *     fraction    = (in_value - input_min) / (input_max - input_min)
 *     out_value   = output_min + fraction * (output_max - output_min)
 *
 *   We still clamp so that if in_value < input_min → return output_min, and
 *   if in_value > input_max → return output_max.
 */
mapping_t M =
		{
				//ENCODER VALUES
				.output_max = 75.0f,
				.output_min = -75.0f,
				//LINEAR VALUES
				.input_max = 180.0f,
				.input_min = -180.0f,

				};

float out_value;

float map_linear( mapping_t M, float in_value)
{





	// Clamp in_value to [input_min .. input_max]
	if (in_value <= M.input_min)
	{
		return M.output_min;
	}
	if (in_value >= M.input_max)
	{
		return M.output_max;
	}

	// Compute fraction in [0.0 .. 1.0]
	float fraction = (in_value - M.input_min) / (M.input_max - M.input_min);
	out_value = M.output_min + fraction * (M.output_max - M.output_min);
	return out_value;
}

