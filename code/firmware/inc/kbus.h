#ifndef __KBUS_H
#define __KBUS_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>

/* Defines -------------------------------------------------------------------*/
#define KBUS_MIN_FRAME_LEN 3
#define KBUS_MAX_FRAME_LEN 36
#define KBUS_MAX_FRAME_SIZE KBUS_MAX_FRAME_LEN + 2

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  KBUS_FrameState KBUS Frame parser state enumeration
  */
typedef enum
{
	CHECK_SRC = 0,
	CHECK_LEN,
	CHECK_CRC
}KBUS_FrameState;
/**
  * @}
  */

/**
  * @brief  KBUS handle Structure definition
  */
typedef struct
{
	uint8_t				framePos;
	uint8_t				frame[KBUS_MAX_FRAME_SIZE];
	KBUS_FrameState		state;
}KBUS_HandleTypeDef;

/**
  * @}
  */

void kbus_init(void);

#endif /* __KBUS_H */
