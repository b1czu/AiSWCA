#include "fifo.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>


/*This initializes the FIFO structure with the given buffer and size*/
int8_t fifo_init(fifo_t * f, uint8_t * buf, uint16_t size)
{
	f->head = 0;
	f->tail = 0;
	f->size = size;
	f->buf = buf;

	if(f->buf == memset(buf, 0, size))
		return FIFO_OK;
	else
		return FIFO_MEM_ERROR;
}

/*This reads nbytes bytes from the FIFO
  The number of bytes read is returned*/
uint16_t fifo_read(fifo_t * f, uint8_t * buf, uint16_t nbytes) 
{
	uint16_t i;
	uint8_t * p;
	p = buf;
	for (i = 0; i < nbytes; i++) {
		if (f->tail != f->head) { //see if any data is available
			*p++ = f->buf[f->tail];  //grab a byte from the buffer
			f->tail++;  //increment the tail
			if (f->tail == f->size) {  //check for wrap-around
				f->tail = 0;
			}
		} else {
			return i; //number of bytes read
		}
	}
	return nbytes;
}

/*This writes up to nbytes bytes to the FIFO
  If the head runs in to the tail, not all bytes are written
  The number of bytes written is returned*/
uint16_t fifo_write(fifo_t * f, const uint8_t * buf, uint16_t nbytes)
{
	uint16_t i;
	const uint8_t * p;
	p = buf;
	for (i = 0; i < nbytes; i++) {
		//first check to see if there is space in the buffer
		if ((f->head + 1 == f->tail)
				|| ((f->head + 1 == f->size) && (f->tail == 0))) {
			return i; //no more room
		} else {
			f->buf[f->head] = *p++;
			f->head++;  //increment the head
			if (f->head == f->size) {  //check for wrap-around
				f->head = 0;
			}
		}
	}
	return nbytes;
}

/*Return actual nuber of elements in fifo*/
static uint16_t fifo_get_actual_element_in_fifo(fifo_t *fifo)
{
	int16_t ret;
	ret = fifo->head - fifo->tail;
	if (ret < 0)
		ret += fifo->size;
	return ret;
}

/*Retrieves, but does not remove, return 0 if fifo is empty
  or in fifo contains not enough data*/
uint16_t fifo_peek(fifo_t * f, uint8_t* buf, uint16_t position) 
{
	if (position < fifo_get_actual_element_in_fifo(f)) {
		*buf = f->buf[(f->tail + position) % f->size]; //grab a byte from the buffer
		return 1;
	} else
		return 0;
}

/*Print fifo content*/

// void fifo_print(fifo_t * f) 
// {
// 	uint8_t* tempPtr = f->buf;

// 	for (int i = 0; i < f->size; ++i) {
// 		printf("%2d ", *tempPtr);
// 		tempPtr++;
// 	}


// 	printf(" *** h %2d t %2d s %2d eif %2d \n", f->head, f->tail, f->size,
// 			fifo_get_actual_element_in_fifo(f));
// }
