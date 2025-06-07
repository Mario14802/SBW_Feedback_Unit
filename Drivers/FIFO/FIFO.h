#ifndef FIFO_FIFO_H_
#define FIFO_FIFO_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int16_t head;   // Next write index
	int16_t tail;   // Next read index
	uint16_t count; // Number of elements in FIFO

	uint16_t Fifo_Size;

	int16_t UserHead;
	int16_t UserTail;
} fifo_T;

void fifo_init(fifo_T *f);

int8_t fifo_is_empty(const fifo_T *f);

int16_t fifo_enqueue(fifo_T *f);

int16_t fifo_dequeue(fifo_T *f);

#endif /* FIFO_FIFO_H_ */
