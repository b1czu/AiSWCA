#include "kbus.h"
#include "main.h"

/*---- Define function ------------------------------------------------*/

/*---- Static variable ------------------------------------------------*/

static UART_HandleTypeDef huart1;
static KBUS_HandleTypeDef hkbus;

/*---- Static function declaration ------------------------------------*/
static void kbus_frame_pos_reset(KBUS_HandleTypeDef* hkbus);
static void kbus_frame_byte_receive(KBUS_HandleTypeDef* hkbus, uint8_t* data);
static void kbus_frame_handler(KBUS_HandleTypeDef* hkbus);
static void kbus_handle_init(KBUS_HandleTypeDef* hkbus);
static void kbus_hw_init(void);

/*---- Function definition --------------------------------------------*/
static void kbus_frame_pos_reset(KBUS_HandleTypeDef* hkbus)
{
	/* Reset KBUS frame position to 0 */
	hkbus->framePos = 0;
	/* Reset KBUS frame parser state to CHECK_SRC */
	hkbus->state = CHECK_SRC;
}

static void kbus_frame_byte_receive(KBUS_HandleTypeDef* hkbus, uint8_t* data)
{
	/* Check if we got less or equal data bytes than KBUS_MAX_FRAME_SIZE in frame buffer */
	if (hkbus->framePos < KBUS_MAX_FRAME_SIZE)
	{
		/* Insert new data into buffer and increment framePos */
		hkbus->frame[hkbus->framePos++] = *data;
	}
	else
	{
		/* This shouldn't ever happen! Sth went wrong */
		LOG_ERROR("%s %d","KBUS_HandleTypeDef->framePos exceeded KBUS_MAX_FRAME_SIZE=",(int)KBUS_MAX_FRAME_SIZE );
	}
}

static void kbus_frame_handler(KBUS_HandleTypeDef* hkbus)
{
	switch(hkbus->state)
	{
	/* Nothing done yet, let's check first byte */
	case CHECK_SRC:
		/* Check if got first (SRC) byte in the KBUS frame buffer */
		if (hkbus->framePos >= 1)
		{
			/* At this stage SRC byte is received and stored in KBUS frame buffer,
			 * Can't do more at this time so let's break and wait for next byte.
			 */
			hkbus->state = CHECK_LEN;
		}
		/* Can't do more at this stage, let's wait for next byte */
		break;
	case CHECK_LEN:
		/* Check if got second (LENGTH) byte in the KBUS frame buffer */
		if (hkbus->framePos >= 2)
		{
			/* Check if LENGTH byte is in (3 - 36) range (KBUS frame spec) */
			if (hkbus->frame[1] >= KBUS_MIN_FRAME_LEN && hkbus->frame[1] <= KBUS_MAX_FRAME_LEN)
			{
				/* At this stage correct LEN byte is received and stored in KBUS frame buffer,
				 * Can't do more at this time so let's break and wait for next byte. */
				hkbus->state = CHECK_CRC;
			}
			else
			{
				/* Invalid LENGTH byte received. Let's start over. */
				kbus_frame_pos_reset(hkbus);
			}
		}
		/* Can't do more at this stage, let's wait for next byte */
		break;
	case CHECK_CRC:
		/* Check if got LENGTH bytes (counted without SRC byte and LEN byte) in the KBUS frame buffer.
		 * If yes, count checksum and check it against checksum received in KBUS frame.
		 * If no, just wait for next byte.
		 * LENGTH is stored in KBUS buffer at frame[1].
		 */
		if (hkbus->framePos >= hkbus->frame[1] + 1)
		{
			uint8_t crc = 0;
			/* Take all bytes without last one (CRC) and count checksum (KBUS frame spec)*/
			for (int i = 0; i <= hkbus->frame[1]; i++)
			{
				crc ^= hkbus->frame[i];
			}
			/* Check if counted checksum match received KBUS frame checksum (which is stored at last byte)*/
			if (hkbus->frame[hkbus->frame[1] + 1] == crc) //xD
			{
				/* YAY! we got good frame ;) Let's add it to known good frame fifo and reset KBUS frame buffer*/
				//TODO add to fifo (maybe pointer to a function stored in hkbus struct that will point to function to execute with received frame as arg?)


				LOG_DEBUG("%s","[KBUS] Frame with good CRC received.");
				/* Good frame received and pushed to FIFO, let's start over */
				kbus_frame_pos_reset(hkbus);
			}
			else
			{
				LOG_DEBUG("%s","[KBUS] Frame with BAD CRC received and dropped.");
				/* Wrong checksum, let's start over */
				kbus_frame_pos_reset(hkbus);
			}
		}
		/* Can't do more at this stage, let's wait for next byte */
		break;
	}
}

static void kbus_handle_init(KBUS_HandleTypeDef* hkbus)
{
	hkbus->framePos = 0;
	hkbus->state = CHECK_SRC;
}

static void kbus_hw_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	huart1.Instance = USART1;
	huart1.Init.BaudRate = 9600;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
	#warning TO DISCUSS
	err_handler(__FILE__,__LINE__,"BUG INFO");
	}

	HAL_NVIC_EnableIRQ(USART1_IRQn);

	__HAL_UART_ENABLE_IT(&huart1, UART_IT_PE);   // Enable the UART Parity Error Interrupt
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_ERR);  // Enable the UART Error Interrupt: (Frame error, noise error, overrun error)
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE); // Enable the UART Data Register not empty Interrupt
}

/* KBUS init function */
void kbus_init(void)
{
	kbus_hw_init();
	kbus_handle_init(&hkbus);
}

/*---- IRQ ------------------------------------------------------------*/

void USART1_IRQHandler(void)
{
  uint8_t rxData;
  uint32_t irq_Status = USART1->ISR;
  if(irq_Status&0x0F)
  {
    #warning ADD ERROR CODE
  }
  else
  {
    if(irq_Status & UART_FLAG_RXNE)
    {                  
      rxData = (uint8_t) USART1->RDR;
      kbus_frame_byte_receive(&hkbus, &rxData);
      kbus_frame_handler(&hkbus);

    }
    else if(irq_Status & UART_FLAG_TXE)
    {

    }
  } 
}
