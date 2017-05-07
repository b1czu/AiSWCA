#include <string.h>
#include "firmware_cfg.h"
#include "cli.h"
#include "dpot.h"
#include "error.h"
#include "kbus.h"
#include "radio_cfg.h"
#include "stm32f070x6.h"
#include "stm32f0xx_hal.h"

/*---- Define function ------------------------------------------------*/

/*---- Static variable ------------------------------------------------*/

static UART_HandleTypeDef huart1;
static KBUS_Handle_t hkbus;

/*---- Static function declaration ------------------------------------*/
static void kbus_frame_byte_receive(KBUS_Handle_t* hkbus, uint8_t* data);
static void kbus_frame_handler(KBUS_Handle_t* hkbus);
static void kbus_handle_init(KBUS_Handle_t* hkbus);
static void kbus_hw_init(void);

/*---- Function definition --------------------------------------------*/
/*This initializes the FIFO structure with the given buffer and size*/
static int8_t kbus_buffer_init(KBUS_BUFFER_t * f, uint8_t * buf, uint16_t size)
{
	f->head = 0;
	f->tail = 0;
	f->size = size;
	f->buf = buf;

	if (f->buf == memset(buf, 0, size))
		return KBUS_BUFFER_OK;
	else
		return KBUS_BUFFER_MEM_ERROR;
}

/*This reads nbytes bytes from the FIFO
 The number of bytes read is returned*/
static uint16_t kbus_buffer_read(KBUS_BUFFER_t * f, uint8_t * buf,
		uint16_t nbytes)
{
	uint16_t i;
	uint8_t * p;
	p = buf;
	for (i = 0; i < nbytes; i++)
	{
		if (f->tail != f->head)
		{ //see if any data is available
			if (p != NULL)
				*p++ = f->buf[f->tail];  //grab a byte from the buffer
			f->tail++;  //increment the tail
			if (f->tail == f->size)
			{  //check for wrap-around
				f->tail = 0;
			}
		}
		else
		{
			return i; //number of bytes read
		}
	}
	return nbytes;
}

/*This writes up to nbytes bytes to the FIFO
 If the head runs in to the tail, not all bytes are written
 The number of bytes written is returned*/
static uint16_t kbus_buffer_write(KBUS_BUFFER_t * f, const uint8_t * buf,
		uint16_t nbytes)
{
	uint16_t i;
	const uint8_t * p;
	p = buf;
	for (i = 0; i < nbytes; i++)
	{
		//first check to see if there is space in the buffer
		if ((f->head + 1 == f->tail)
				|| ((f->head + 1 == f->size) && (f->tail == 0)))
		{
			return i; //no more room
		}
		else
		{
			f->buf[f->head] = *p++;
			f->head++;  //increment the head
			if (f->head == f->size)
			{  //check for wrap-around
				f->head = 0;
			}
		}
	}
	return nbytes;
}

/*Return actual amount of elements in buffer*/
static uint16_t kbus_buffer_get_elements_amount(KBUS_BUFFER_t *fifo)
{
	int16_t ret;
	ret = fifo->head - fifo->tail;
	if (ret < 0)
		ret += fifo->size;
	return ret;
}

/*Retrieves, but does not remove, return 0 if fifo is empty
 or if fifo contains not enough data*/
static uint16_t kbus_buffer_peek(KBUS_BUFFER_t * f, uint8_t* buf,
		uint16_t position)
{
	if (position < kbus_buffer_get_elements_amount(f))
	{
		*buf = f->buf[(f->tail + position) % f->size]; //grab a byte from the buffer
		//LOG_DEBUG("\nDEBUG_kbus_buffer_peek: 0x%02X\n",*buf);
		return 1;
	}
	else
		return 0;
}

static void kbus_frame_byte_receive(KBUS_Handle_t* hkbus, uint8_t* data)
{
	/* Insert new data into buffer */
	kbus_buffer_write(&hkbus->kFifo, data, sizeof(uint8_t));
}

static void kbus_frame_parser(uint8_t* frame_p, uint8_t frame_len )
{
	if(!memcmp(frame_p, KBUS_KEY_IN, frame_len + 2)){
		LOG_INFO("KBUS_KEY_IN");
		return;
	}
	if(!memcmp(frame_p, KBUS_KEY_OUT, frame_len + 2)){
		LOG_INFO("KBUS_KEY_OUT");
		return;
	}
	if(!memcmp(frame_p, KBUS_MFL_VOL_UP, frame_len + 2)){
		dpot_set_value(DPOT_ADDRESS_POT_0, RADIO_VOLUME_UP_BTN);
		dpot_blip(200);
		LOG_INFO("KBUS_MFL_VOL_UP pressed");
		return;
	}
	if(!memcmp(frame_p, KBUS_MFL_VOL_DOWN, frame_len + 2)){
		dpot_set_value(DPOT_ADDRESS_POT_0, RADIO_VOLUME_DOWN_BTN);
		dpot_blip(200);
		LOG_INFO("KBUS_MFL_VOL_DOWN pressed");
		return;
	}
	if(!memcmp(frame_p, KBUS_MFL_NEXT_PRESS, frame_len + 2)){
		dpot_set_value(DPOT_ADDRESS_POT_0, RADIO_TUNE_UP_BTN);
		dpot_blip(900);
		LOG_INFO("KBUS_MFL_NEXT_PRESS pressed");
		return;
	}
	if(!memcmp(frame_p, KBUS_MFL_NEXT_PRESS_LONG, frame_len + 2)){
		dpot_blip_cancel();
		dpot_set_value(DPOT_ADDRESS_POT_0, RADIO_TUNE_UP_BTN);
		dpot_enable();
		LOG_INFO("KBUS_MFL_NEXT_PRESS_LONG pressed");
		return;
	}
	if(!memcmp(frame_p, KBUS_MFL_NEXT_RELEASE, frame_len + 2)){
		LOG_INFO("KBUS_MFL_NEXT_RELEASE released");
		dpot_disable();
		return;
	}
	if(!memcmp(frame_p, KBUS_MFL_PREVIOUS_PRESS, frame_len + 2)){
		dpot_set_value(DPOT_ADDRESS_POT_0, RADIO_TUNE_DOWN_BTN);
		dpot_blip(900);
		LOG_INFO("KBUS_MFL_PREVIOUS_PRESS pressed");
		return;
	}
	if(!memcmp(frame_p, KBUS_MFL_PREVIOUS_PRESS_LONG, frame_len + 2)){
		dpot_blip_cancel();
		dpot_set_value(DPOT_ADDRESS_POT_0, RADIO_TUNE_DOWN_BTN);
		dpot_enable();
		LOG_INFO("KBUS_MFL_PREVIOUS_PRESS_LONG pressed");
		return;
	}
	if(!memcmp(frame_p, KBUS_MFL_PREVIOUS_RELEASE, frame_len + 2)){
		LOG_INFO("KBUS_MFL_PREVIOUS_RELEASE released");
		dpot_disable();
		return;
	}
	if(!memcmp(frame_p, KBUS_MFL_SEND_END_PRESS, frame_len + 2)){
		LOG_INFO("KBUS_MFL_SEND_END_PRESS pressed");
		dpot_set_value(DPOT_ADDRESS_POT_0, RADIO_SOURCE_BTN);
		dpot_blip(900);
		return;
	}
	if(!memcmp(frame_p, KBUS_MFL_SEND_END_PRESS_LONG, frame_len + 2)){
		dpot_blip_cancel();
		dpot_set_value(DPOT_ADDRESS_POT_0, RADIO_SOURCE_BTN);
		dpot_enable();
		LOG_INFO("KBUS_MFL_SEND_END_PRESS_LONG pressed");
		return;
	}
	if(!memcmp(frame_p, KBUS_MFL_SEND_END_RELEASE, frame_len + 2)){
		LOG_INFO("KBUS_MFL_SEND_END_RELEASE released");
		dpot_disable();
		return;
	}
	if(!memcmp(frame_p, KBUS_MFL_RT_PRESS, frame_len + 2)){
		dpot_set_value(DPOT_ADDRESS_POT_0, RADIO_ATT_BTN);
		dpot_blip(200);
		LOG_INFO("KBUS_MFL_RT_PRESS pressed");
		return;
	}
	if(!memcmp(frame_p, KBUS_IGNITION_OFF, frame_len + 2)){
		LOG_INFO("KBUS_IGNITION_OFF");
		return;
	}
	if(!memcmp(frame_p, KBUS_IGNITION_POS1, frame_len + 2)){
		LOG_INFO("KBUS_IGNITION_POS1");
		return;
	}
	if(!memcmp(frame_p, KBUS_IGNITION_POS2, frame_len + 2)){
		LOG_INFO("KBUS_IGNITION_POS2");
		return;
	}
}

static void kbus_frame_handler(KBUS_Handle_t* hkbus)
{
	uint8_t frameSrc = 0;
	uint8_t frameSize = 0;
	switch (hkbus->kState)
	{
	/* Nothing done yet, let's check first byte */
	case CHECK_SRC:
		/* Check if got first (SRC) byte in the KBUS frame buffer */
		if (kbus_buffer_get_elements_amount(&hkbus->kFifo) >= 1)
		{
			/* At this stage SRC byte is received and stored in KBUS frame buffer,
			 * Let's check source address and if it is something that we are interested in
			 */
			if (kbus_buffer_peek(&hkbus->kFifo, &frameSrc, 0) == 0)
			{
				//no data in buff ?? shouldnt happen after earlier checks
				break;
			}
/*			if (frameSrc != KBUS_ADDR_MFL && frameSrc != KBUS_ADDR_IMO && frameSrc != KBUS_ADDR_IKE)//we are only interested in data from MFL, IMO or KMBI
			{
				//LOG_DEBUG("%s", "SRC addr is not MFL! Removing one position from FIFO...");
				kbus_buffer_read(&hkbus->kFifo, NULL, 1);//Wrong source address, removing one byte from buffer.
				break;
			}*/
			hkbus->kState = CHECK_LEN;
			//LOG_DEBUG("%s", "kbus->kState = CHECK_LEN;");
		}
		/* Can't do more at this stage, let's wait for next byte */
	case CHECK_LEN:
		/* Check if got second (LENGTH) byte in the KBUS frame buffer */
		if (kbus_buffer_get_elements_amount(&hkbus->kFifo) >= 2)
		{

			if (kbus_buffer_peek(&hkbus->kFifo, &frameSize, 1) == 0)
			{
				hkbus->kState = CHECK_SRC;
				break;
			}
			/* Check if LENGTH byte is in (3 - 36) range (KBUS frame spec) */
			if (frameSize >= KBUS_MIN_FRAME_LEN
					&& frameSize <= KBUS_MAX_FRAME_LEN)
			{
				/* At this stage correct LEN byte is received and stored in KBUS frame buffer,
				 * Can't do more at this time so let's break and wait for next byte. */
				hkbus->kState = CHECK_CRC;
				//LOG_DEBUG("Good LENGTH byte! (%d bytes)", frameSize);
			}
			else
			{
				/* Invalid LENGTH byte received. Let's move by one byte and start over. */
				//LOG_DEBUG("%s", "Invalid LENGTH byte! Removing one position from FIFO...");
				kbus_buffer_read(&hkbus->kFifo, NULL, 1);
				hkbus->kState = CHECK_SRC;
				break;
			}
		}
		/* Can't do more at this stage, let's wait for next byte */
	case CHECK_CRC:
		/* Check if got LENGTH bytes (counted without SRC byte and LEN byte) in the KBUS frame buffer.
		 * If yes, count checksum and check it against checksum received in KBUS frame.
		 * If no, just wait for next byte.
		 * LENGTH is stored in KBUS buffer at offset[1].
		 */
		kbus_buffer_peek(&hkbus->kFifo, &frameSize, 1);
		if (kbus_buffer_get_elements_amount(&hkbus->kFifo) >= frameSize + 2)
		{
			uint8_t countedCRC = 0;
			uint8_t frameCRC = 0;
			uint8_t byte = 0;

			/* Take all bytes without last one (CRC) and count checksum (as told in KBUS frame specs)*/
			for (int i = 0; i <= frameSize; i++)
			{
				kbus_buffer_peek(&hkbus->kFifo, &byte, i);
				countedCRC ^= byte;
			}
			/* Check if counted checksum match received KBUS frame checksum (which is stored at last byte)*/
			kbus_buffer_peek(&hkbus->kFifo, &frameCRC, frameSize + 1);
			if (frameCRC == countedCRC)
			{
				uint8_t frame[KBUS_MAX_FRAME_SIZE];
				/* YAY! we got good frame ;) */
				//TODO add to fifo (maybe pointer to a function stored in hkbus struct that will point to function to execute with received frame as arg?)
				// NOPE!!!!!!
				//TODO add parser of known frames (maybe struct with pointers to functions telling what to do when sth happens?)

				/* take frame from buffer */
				kbus_buffer_read(&hkbus->kFifo, &frame[0], frameSize + 2);

				#if DEBUG_LOG
				/* ugly print */
				char frame_as_text[200];
				char*  frame_as_text_p = frame_as_text;
				for(int c = 0; c < frameSize + 2; c++)
				{
					if(frame[c] < 0x10)
						frame_as_text_p += sprintf_(frame_as_text_p, "0");
					frame_as_text_p += sprintf_(frame_as_text_p, "%x ", frame[c]);
				}
				LOG_DEBUG("%s %s","[KBUS] Frame with good CRC received.", frame_as_text);
				#endif
				/* send frame to parser function */
				kbus_frame_parser(&frame[0], frameSize);
				/* Good frame received and processed, let's start over */
				hkbus->kState = CHECK_SRC;
			}
			else
			{
				/* Wrong checksum, let's move by one byte and start over */
				LOG_ERROR("%s","[KBUS] Frame with BAD CRC received and dropped.");
				kbus_buffer_read(&hkbus->kFifo, NULL, 1);
				hkbus->kState = CHECK_SRC;
			}
		}
		else
		{
			//LOG_DEBUG("Wait for more data (Have %d need %d)", (int)kbus_buffer_get_elements_amount(&hkbus->kFifo), frameSize + 2 );
		}
		/* Can't do more at this stage, let's wait for next byte */
		break;
	}
}

static void kbus_handle_init(KBUS_Handle_t* hkbus)
{
	hkbus->kState = CHECK_SRC;
	kbus_buffer_init(&hkbus->kFifo, hkbus->kBuffer, sizeof(hkbus->kBuffer));
}

static void kbus_hw_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_USART1_CLK_ENABLE()
	;
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;

	GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	huart1.Instance = USART1;
	huart1.Init.BaudRate = 9600;
	huart1.Init.WordLength = UART_WORDLENGTH_9B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_EVEN;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
#warning TO DISCUSS
		err_handler(__FILE__, __LINE__, "BUG INFO");
	}

	__HAL_UART_ENABLE_IT(&huart1, UART_IT_PE); // Enable the UART Parity Error Interrupt
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_ERR); // Enable the UART Error Interrupt: (Frame error, noise error, overrun error)
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE); // Enable the UART Data Register not empty Interrupt

	HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/* KBUS init function */
void kbus_init(void)
{
	kbus_hw_init();
	kbus_handle_init(&hkbus);
	LOG_INFO("KBUS support initialized");
}

void kbus_logic(void)
{
	kbus_frame_handler(&hkbus);
}

/*---- IRQ ------------------------------------------------------------*/

void USART1_IRQHandler(void)
{
	/* UART parity error interrupt occurred -------------------------------------*/
	if ((__HAL_UART_GET_IT(&huart1, UART_IT_PE) != RESET)
			&& (__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_PE) != RESET))
	{
		__HAL_UART_CLEAR_PEFLAG(&huart1);

		LOG_ERROR("KBUS PARITY ERROR\n");
	}

	/* UART frame error interrupt occurred --------------------------------------*/
	if ((__HAL_UART_GET_IT(&huart1, UART_IT_FE) != RESET)
			&& (__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_ERR) != RESET))
	{
		__HAL_UART_CLEAR_FEFLAG(&huart1);

		LOG_ERROR("KBUS FRAME ERROR\n");
	}

	/* UART noise error interrupt occurred --------------------------------------*/
	if ((__HAL_UART_GET_IT(&huart1, UART_IT_NE) != RESET)
			&& (__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_ERR) != RESET))
	{
		__HAL_UART_CLEAR_NEFLAG(&huart1);

		LOG_ERROR("KBUS NOISE ERROR\n");
	}

	/* UART Over-Run interrupt occurred -----------------------------------------*/
	if ((__HAL_UART_GET_IT(&huart1, UART_IT_ORE) != RESET)
			&& (__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_ERR) != RESET))
	{
		__HAL_UART_CLEAR_OREFLAG(&huart1);

		LOG_ERROR("KBUS OVERRUN ERROR\n");
	}

	if ((__HAL_UART_GET_IT(&huart1, UART_IT_RXNE) != RESET)
			&& (__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_RXNE) != RESET))
	{

		/* we got a character */
		uint8_t rxData = (uint8_t) (huart1.Instance->RDR & 0xff);
		kbus_frame_byte_receive(&hkbus, &rxData);

		/* Clear RXNE interrupt flag */
		__HAL_UART_SEND_REQ(&huart1, UART_RXDATA_FLUSH_REQUEST);
	}

	/* UART in mode Transmitter ------------------------------------------------*/
	if ((__HAL_UART_GET_IT(&huart1, UART_IT_TXE) != RESET)
			&& (__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_TXE) != RESET))
	{
		;
	}

	/* UART in mode Transmitter (transmission end) -----------------------------*/
	if ((__HAL_UART_GET_IT(&huart1, UART_IT_TC) != RESET)
			&& (__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_TC) != RESET))
	{
		;
	}
}
