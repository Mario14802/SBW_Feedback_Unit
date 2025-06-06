/*
 * Encoder.c
 *
 *  Created on: Jun 5, 2025
 *      Author: MARIO
 */


#include "Encoder.h"
float encoder_angle(int32_t Timer_counter)
{
	int32_t encoder_count = 0;
	float encoder_angle = 0.0f;

	encoder_count =  Timer_counter;

	// Convert to angle
	encoder_angle = ((float)encoder_count /PPR) * 360.0f;

	// Optional: wrap angle to [0, 360)
	if (encoder_angle >= 360.0f)
		encoder_angle -= 360.0f;
	else if (encoder_angle < 0.0f)
		encoder_angle += 360.0f;

	return encoder_angle;


}
