/*
 * Feedback_CAN.h
 *
 *  Created on: Apr 27, 2025
 *      Author: M.W Laptop
 */

#ifndef INC_FEEDBACK_CAN_H_
#define INC_FEEDBACK_CAN_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define TX_SIZE 8

void Encode(uint8_t *txdata, uint8_t start_bit, uint8_t length, uint16_t value,
		uint8_t IsSigned);

uint16_t Decode(uint8_t *rxdata, uint8_t start_bit, uint8_t length,
		uint8_t IsSigned);

void PrepareCANMessage(uint8_t *txdata, int16_t motor_current,
		int16_t steering_wheel_angle, int16_t steering_wheel_speed,
		int16_t PWM_output);

#endif /* INC_FEEDBACK_CAN_H_ */
