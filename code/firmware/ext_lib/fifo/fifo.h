#ifndef __FIFO_H
#define __FIFO_H

#include <stdint.h>

typedef enum fifo_return_code_e{
	FIFO_OK = 0,
	FIFO_MEM_ERROR,
}fifo_return_code_t;

typedef struct fifo_t {
	uint8_t * buf;
	uint16_t head;
	uint16_t tail;
	uint16_t size;
} fifo_t;


/*This initializes the FIFO structure with the given buffer and size*/
int8_t fifo_init(fifo_t * f, uint8_t * buf, uint16_t size);


/*This reads nbytes bytes from the FIFO
  The number of bytes read is returned*/
uint16_t fifo_read(fifo_t * f, uint8_t * buf, uint16_t nbytes);

/*This writes up to nbytes bytes to the FIFO
  If the head runs in to the tail, not all bytes are written
  The number of bytes written is returned*/
uint16_t fifo_write(fifo_t * f, const uint8_t * buf, uint16_t nbytes);

/*Retrieves, but does not remove, return 0 if fifo is empty
  or in fifo contains not enough data*/
uint16_t fifo_peek(fifo_t * f, uint8_t* buf, uint16_t position);

/*Print fifo content, by default, the function is disabled*/
//void fifo_print(fifo_t * f);

#endif /* __FIFO_H */






