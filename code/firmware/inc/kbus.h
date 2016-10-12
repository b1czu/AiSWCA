#ifndef __KBUS_H
#define __KBUS_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

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

/* KBUS message definitions */
static const uint8_t KBUS_KEY_IN[7]  =
{  0x44, 0x05, 0xBF, 0x74, 0x04, 0x00, 0x8E }; // Ignition key in
static const uint8_t KBUS_KEY_OUT[7]  =
{  0x44, 0x05, 0xBF, 0x74, 0x00, 0xFF, 0x75 }; // Ignition key out
static const uint8_t KBUS_MFL_VOL_UP[6]  =
{  0x50, 0x04, 0x68, 0x32, 0x11, 0x1F }; // Steering wheel Volume Up
static const uint8_t KBUS_MFL_VOL_DOWN[6]  =
{  0x50, 0x04, 0x68, 0x32, 0x10, 0x1E }; // Steering wheel Volume Down
static const uint8_t KBUS_MFL_NEXT_PRESS[6]  =
{  0x50, 0x04, 0x68, 0x3B, 0x01, 0x06 }; // Steering wheel next press
static const uint8_t KBUS_MFL_NEXT_PRESS_LONG[6]  =
{  0x50, 0x04, 0x68, 0x3B, 0x11, 0x16 }; // Steering wheel next long press
static const uint8_t KBUS_MFL_NEXT_RELEASE[6]  =
{  0x50, 0x04, 0x68, 0x3B, 0x21, 0x26 }; // Steering wheel next release
static const uint8_t KBUS_MFL_PREVIOUS_PRESS[6]  =
{  0x50, 0x04, 0x68, 0x3B, 0x08, 0x0F }; // Steering wheel previous press
static const uint8_t KBUS_MFL_PREVIOUS_PRESS_LONG[6]  =
{  0x50, 0x04, 0x68, 0x3B, 0x18, 0x1F }; // Steering wheel previous long press
static const uint8_t KBUS_MFL_PREVIOUS_RELEASE[6]  =
{  0x50, 0x04, 0x68, 0x3B, 0x28, 0x2F }; // Steering wheel previous release
static const uint8_t KBUS_MFL_SEND_END_PRESS[6]  =
{  0x50, 0x04, 0xC8, 0x3B, 0x80, 0x27 }; // Steering wheel send/end press
static const uint8_t KBUS_MFL_SEND_END_PRESS_LONG[6]  =
{  0x50, 0x04, 0xC8, 0x3B, 0x90, 0x37 }; // Steering wheel send/end long press
static const uint8_t KBUS_MFL_SEND_END_RELEASE[6]  =
{  0x50, 0x04, 0xC8, 0x3B, 0x90, 0x37 }; // Steering wheel send/end release
static const uint8_t KBUS_MFL_RT_PRESS[5]  =
{  0x50, 0x03, 0xC8, 0x01, 0x9A }; // MFL R/T press
static const uint8_t KBUS_IGNITION_OFF[6]  =
{  0x80, 0x04, 0xBF, 0x11, 0x00, 0x2A }; // Ignition Off
static const uint8_t KBUS_IGNITION_POS1[6]  =
{  0x80, 0x04, 0xBF, 0x11, 0x01, 0x2B }; // Ignition Acc position - POS1
static const uint8_t KBUS_IGNITION_POS2[6]  =
{  0x80, 0x04, 0xBF, 0x11, 0x03, 0x29 }; // Ignition On position - POS2
static const uint8_t KBUS_REMOTE_UNLOCK[6]  =
{  0x00, 0x04, 0xBF, 0x72, 0x22, 0xEB }; // Remote control unlock

#endif /* __KBUS_H */
