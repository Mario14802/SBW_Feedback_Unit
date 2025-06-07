#include "SbW_protocol.h"
#include <string.h>

/**
 * @fn uint16_t CRC16(uint8_t*, uint16_t)
 * @brief
 * Computes the CRC value of a certain data frame and returns it
 * @param data ptr to the data buffer
 * @param len number of bytes for the CRC to be calculated for
 * @return CRC byte value
 */
static uint16_t CRC16(uint8_t *data, uint16_t len) {
	uint16_t CRC = 0;
	for (uint16_t x = 0; x < len; x++) {
		CRC += data[x];
	}
	return CRC;
}

void SbW_Init(SbW_Protocol_t *S) {
	S->MessageFifo.Fifo_Size = S->Fifo_Buffer_Size / S->Frame_Len;
	fifo_init(&S->MessageFifo);
	S->Mutex = 0; //ensures that the Mutex flag starts in the unlocked state (0)
}

void SbW_Request_Received_CB(SbW_Protocol_t *S, uint8_t *data, uint16_t len) {
	uint8_t NoReply = 0;
	uint16_t CRC_Result = CRC16(data, len - 2);
	if (S->RemainingFrames) {
		return;
	}
	// Check the data integrity
	if (CRC_Result
			!= (((uint16_t) data[len - 1] << 8) | (uint16_t) data[len - 2])) {
		S->HW_Interface_t.User_Callback(SbW_ERROR_CRC);
		// SbW_ERROR_CRC // Ignoring this frame
	}
	// SbW_ERROR_NoERROR
	S->R_W = data[1] >> 7;
	S->CMD = data[1] & 0b01111111;

	switch (S->CMD) {
	case 0x01: // Get or Set Sampling Frequency
		S->HW_Interface_t.User_Callback(SbW_ERROR_NoERROR);
		if (S->R_W) { // Get Request (ECU -> PC)
			len = 7;
			data[2] = 2; // Number of Data Bytes =2
			data[3] = (uint8_t) (S->SamplingFreq >> 8);
			data[4] = (uint8_t) S->SamplingFreq;
			// data[5] | data[6] for CRC_Result
		} else { // Set Request (ECU -> PC)
			len = 5;
			S->SamplingFreq = ((uint16_t) data[3] << 8) | (uint16_t) data[4];
			data[2] = 0; // Number of Data Bytes =0
			// data[3] | data[4] for CRC_Result
		}
		break;

	case 0x02: // Get or Set Frame Length
		S->HW_Interface_t.User_Callback(SbW_ERROR_NoERROR);
		if (S->R_W) { // Get Request (ECU -> PC)
			len = 6;
			data[2] = 1; // Number of Data Bytes =1
			data[3] = S->Frame_Len; // Get Frame_Len
			// data[4] | data[5] for CRC_Result
		} else { // Set Request (ECU -> PC)
			len = 5;
			S->Frame_Len = data[3]; // Set Frame_Len
			data[2] = 0; // Number of Data Bytes =0
			// data[3] | data[4] for CRC_Result
		}
		break;
	case 0x03: // Get or Set Stream ON
		S->HW_Interface_t.User_Callback(SbW_ERROR_NoERROR);
		if (S->R_W) { // Get Request (ECU -> PC)
			len = 6;
			data[2] = 1; // Number of Data Bytes =1
			data[3] = S->Stream_ON; // Get Stream_ON
			// data[4] | data[5] for CRC_Result
		} else { // Set Request (ECU -> PC)
			len = 5;
			S->Stream_ON = data[3]; // Set Stream_ON
			data[2] = 0; // Number of Data Bytes =0
			// data[3] | data[4] for CRC_Result
		}
		break;
	case 0x04: // Get number of frames to be reported by the controller
		S->HW_Interface_t.User_Callback(SbW_ERROR_NoERROR);
		if (!S->R_W) { // ??
			NoReply = 1; // means do not issue a standard reply
			S->RemainingFrames = data[3];
			// SbW_TxFrame_processor(S);
		}
		break;

	case 0x06: // Get the frame buffer depth
		S->HW_Interface_t.User_Callback(SbW_ERROR_NoERROR);
		if (S->R_W) { // Get Request (ECU -> PC)
			len = 7;
			uint16_t FrameBufferDepth = S->Fifo_Buffer_Size / S->Frame_Len;
			data[3] = (uint8_t) (FrameBufferDepth >> 8);
			data[4] = (uint8_t) FrameBufferDepth;
		}
		break;

	default:
		return;
	}

	if (!NoReply) {
		CRC_Result = CRC16(data, len - 2);
		data[len - 2] = (uint8_t) (CRC_Result >> 8);
		data[len - 1] = (uint8_t) (CRC_Result);
		S->HW_Interface_t.Send_Reply(data, len);
	}
}
void SbW_Reply_Transmit(SbW_Protocol_t *S, uint8_t *data, uint16_t len) {

}

void SbW_Timer_Callback(SbW_Protocol_t *S) {
	if (!S->Mutex) {
		S->Mutex = 1;
		int16_t Head = fifo_enqueue(&S->MessageFifo);
		// Implement the enqueue operation

		memcpy(S->Fifo_Buffer + (Head * S->Frame_Len), S->FrameDataBaseAddress,
				S->Frame_Len);
		S->Mutex = 0;
		SbW_TxFrame_processor(S);
	}
}

void SbW_TxFrame_processor(SbW_Protocol_t *S) {
	if (!fifo_is_empty(&S->MessageFifo) && S->RemainingFrames != 0
			&& !S->Mutex) {
		S->Mutex = 1;
		int16_t H = fifo_dequeue(&S->MessageFifo);
		S->RemainingFrames--;
		memcpy(S->P_TXBuffer, S->Fifo_Buffer + (H * S->Frame_Len),
				S->Frame_Len);
		S->HW_Interface_t.Send_Reply(S->P_TXBuffer, S->Frame_Len);
		S->Mutex = 0;
	}
}

void SbW_TxCPLt(SbW_Protocol_t *S) {
	SbW_TxFrame_processor(S);
}
