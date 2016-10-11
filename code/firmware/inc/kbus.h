#ifndef __KBUS_H
#define __KBUS_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>

/* Defines -------------------------------------------------------------------*/
#define KBUS_MIN_FRAME_LEN 3
#define KBUS_MAX_FRAME_LEN 36
#define KBUS_MAX_FRAME_SIZE (KBUS_MAX_FRAME_LEN + 2)

#define KBUS_ADDR_MFL 0x50	//MFL
#define KBUS_ADDR_RAD 0x68	//RAD
#define KBUS_ADDR_CDC 0x18	//CDC
#define KBUS_ADDR_DSP 0x6A	//DSP
#define KBUS_ADDR_TEL 0xC8	//TEL
#define KBUS_ADDR_SES 0xB0	//SES
#define KBUS_ADDR_RLS 0xE8	//RLS

/* Exported types ------------------------------------------------------------*/
/**
 * @brief  KBUS buffer Structure definition
 */
typedef struct KBUS_BUFFER_t
{
	uint8_t * buf;
	uint16_t head;
	uint16_t tail;
	uint16_t size;
} KBUS_BUFFER_t;
/**
 * @}
 */

/**
 * @brief  KBUS buffer error Structure definition
 */
typedef enum KBUS_BUFFER_return_code_e
{
	KBUS_BUFFER_OK = 0, KBUS_BUFFER_MEM_ERROR
} KBUS_BUFFER_return_code_e;
/**
 * @}
 */

/**
 * @brief  KBUS_FrameState KBUS Frame parser state enumeration
 */
typedef enum
{
	CHECK_SRC = 0, CHECK_LEN, CHECK_CRC
} KBUS_FrameState;
/**
 * @}
 */

/**
 * @brief  KBUS handle Structure definition
 */
typedef struct
{
	uint8_t kBuffer[KBUS_MAX_FRAME_SIZE * 5]; //buffer space for 5 frames of maximum size
	KBUS_BUFFER_t kFifo;
	KBUS_FrameState kState;
} KBUS_Handle_t;
/**
 * @}
 */

void kbus_init(void);
void kbus_logic(void);

#endif /* __KBUS_H */
