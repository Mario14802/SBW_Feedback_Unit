/*
 * Feedback_CAN.c
 *
 *  Created on: Apr 27, 2025
 *      Author: M.W Laptop
 */

#include "Feedback_CAN.h"
void Encode(uint8_t *txdata, uint8_t start_bit, uint8_t length,
		uint16_t value, uint8_t IsSigned) {

	/*if (offset != 0) {
		value /= offset;
	}
*/
	// If signed, move the sign bit to the last bit of the data field
	uint16_t sign = (value & (1 << 15)) ? 1 : 0; //if sign is 1 : negative value
	uint16_t mask = (length < 16) ? ((1 << length) - 1) : 0xFFFF; // to avoid over flow
	uint16_t newValue = (value & (mask >> 1)) | (sign << (length - 1));
	//new value = value after removing not using bit

	// Insert value into txdata at correct bit position
	for (uint8_t i = 0; i < length; i++) {
		uint8_t byte_index = (start_bit + i) / 8;  // `txdata`byte postion
		uint8_t bit_position = (start_bit + i) % 8; // bit postion

		if (newValue & (1 << i)) {
			txdata[byte_index] |= (1 << bit_position);
		} else {
			txdata[byte_index] &= ~(1 << bit_position);
		}
	}
}

// decode function
uint16_t Decode(uint8_t *rxdata, uint8_t start_bit,
		uint8_t length, uint8_t IsSigned) {
	uint16_t value = 0;
	for (uint8_t i = 0; i < length; i++) {
		uint8_t byte_index = (start_bit + i) / 8;
		uint8_t bit_position = (start_bit + i) % 8;
		if (rxdata[byte_index] & (1 << bit_position)) {
			value |= (1 << i);
		}
	}
	if (IsSigned && (value & (1 << (length - 1)))) {
		value |= ~((1 << length) - 1);
	}
	/*if (offset != 0) {
		value *= offset;

	}*/

	return value;
}

// Function to prepare CAN message
void PrepareCANMessage(uint8_t *txdata, int16_t motor_current,
		int16_t steering_wheel_angle, int16_t steering_wheel_speed,
		int16_t PWM_output) {
	memset(txdata, 0, TX_SIZE); // Clear txdata before encoding ***********************************

	// Encode parameters into the message
	Encode(txdata, 0, 13, motor_current, 1);   // Speed: 12-bit, unsigned
	Encode(txdata, 13,  12, steering_wheel_angle, 1); // Angle: 10-bit, signed
	Encode(txdata, 25,  14, steering_wheel_speed, 1); // Current: 8-bit, unsigned
	Encode(txdata, 39,  15, PWM_output, 1);
}
