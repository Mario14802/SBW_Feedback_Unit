/*
 * Encoder.h
 *
 *  Created on: Jun 5, 2025
 *      Author: MARIO
 */

#ifndef INCREMNTAL_ENCODER_ENCODER_H_
#define INCREMNTAL_ENCODER_ENCODER_H_

#include <stdint.h>


#define PPR 2400

#define ANGLE_PER_COUNT  (360.0f / PPR)
/*
typedef struct {
    int32_t prev_count;
    float   angle_accum;
} EncoderState;
*/

float get_encoder_angle(int32_t Timer_counter);

#endif /* INCREMNTAL_ENCODER_ENCODER_H_ */
