#ifndef SbW_PROTOCOL_SbW_PROTOCOL_H_
#define SbW_PROTOCOL_SbW_PROTOCOL_H_

#include <stdint.h>
#include "../FIFO/FIFO.h"

typedef enum SbW_Err_Codes {
	SbW_ERROR_NoERROR, SbW_ERROR_CRC,
} SbW_Err_Codes_t;

typedef struct SbW_Protocol {
	uint8_t Stream_ON :1;
	uint16_t SamplingFreq;
	uint8_t R_W :1; // R_W:0 -> Write, R_W:1 -> Read
	uint8_t CMD;

	// User frame length
	uint8_t Frame_Len;

	// Pointer to the user frame base address
	uint8_t *FrameDataBaseAddress;

	// FIFO containing the head and tail
	fifo_T MessageFifo;

	// Pointer to the frame buffer where the data will be placed
	uint8_t *Fifo_Buffer;

	// Size of the buffer
	uint16_t Fifo_Buffer_Size;

	// Pointer to the communication TX buffer where data will be placed
	uint8_t *P_TXBuffer;

	struct Hw_Interface {
		void (*Send_Reply)(uint8_t *data, uint16_t len);
		void (*User_Callback)(SbW_Err_Codes_t Error_Code);
	} HW_Interface_t;

	uint8_t RemainingFrames;

	// Software flag ensures that the function's body executes only
	// if no other process is currently using this transmission logic
	uint8_t Mutex;
} SbW_Protocol_t;

// Initialize the protocol
void SbW_Init(SbW_Protocol_t *S);

// This function will be called back from the USB (CDC_Receive_FS)
// to pass the received data to the Software driver (SbW_Protocol)
void SbW_Request_Received_CB(SbW_Protocol_t *S, uint8_t *data, uint16_t len);

// Reply to the controller from the protocol
void SbW_Reply_Transmit(SbW_Protocol_t *S, uint8_t *data, uint16_t len);

// To be called from the timer ISR (HAL_TIM_PeriodElapsedCallback)
// Inside it the data will be enqueued inside the FIFO
// Enqueues the current frame (from FrameDataBaseAddress) into a FIFO buffer.
// Calls the transmission processor to try sending it.
void SbW_Timer_Callback(SbW_Protocol_t *S);

// Transmit the next message in the FIFO, if conditions are allowed
void SbW_TxFrame_processor(SbW_Protocol_t *S);

// Called when a transmission is complete
// This allows continuous streaming of frames from the FIFO as each one completes
void SbW_TxCPLt(SbW_Protocol_t *S);
#endif /* SbW_PROTOCOL_SbW_PROTOCOL_H_ */
