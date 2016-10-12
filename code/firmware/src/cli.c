#include "cli.h"
#include "main.h"

/*---- Define function -----------------------------------------------*/

/*---- Static variable -----------------------------------------------*/

static UART_HandleTypeDef huart2;

/*---- Static function declaration -----------------------------------*/

static void cli_hw_init(void);
static void cli_logic_init(void);

/*---- Function definition --------------------------------------------*/

void cli_init(void)
{
	cli_hw_init();
	cli_logic_init();
}

/* USART2 init function */
static void cli_hw_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_USART2_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
  	#warning TODO
    err_handler(__FILE__,__LINE__,"BUG INFO");
  }

  HAL_NVIC_EnableIRQ(USART2_IRQn);  
  
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_PE);   // Enable the UART Parity Error Interrupt
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_ERR);  // Enable the UART Error Interrupt: (Frame error, noise error, overrun error)
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE); // Enable the UART Data Register not empty Interrupt 
}

static void cli_logic_init(void)
{

}


void cli_put_char(char ch)
{
	char temp[1];
	temp[0] = ch;
	HAL_UART_Transmit(&huart2,&temp[0],sizeof(temp),100);
}

/*---- IRQ ------------------------------------------------------------*/

void USART2_IRQHandler(void){
	/* UART parity error interrupt occurred -------------------------------------*/
	if ((__HAL_UART_GET_IT(&huart2, UART_IT_PE) != RESET)
			&& (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_PE) != RESET))
	{
		__HAL_UART_CLEAR_PEFLAG(&huart2);

		LOG_ERROR("UART PARITY ERROR\n");
	}

	/* UART frame error interrupt occurred --------------------------------------*/
	if ((__HAL_UART_GET_IT(&huart2, UART_IT_FE) != RESET)
			&& (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_ERR) != RESET))
	{
		__HAL_UART_CLEAR_FEFLAG(&huart2);

		LOG_ERROR("UART FRAME ERROR\n");
	}

	/* UART noise error interrupt occurred --------------------------------------*/
	if ((__HAL_UART_GET_IT(&huart2, UART_IT_NE) != RESET)
			&& (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_ERR) != RESET))
	{
		__HAL_UART_CLEAR_NEFLAG(&huart2);

		LOG_ERROR("UART NOISE ERROR\n");
	}

	/* UART Over-Run interrupt occurred -----------------------------------------*/
	if ((__HAL_UART_GET_IT(&huart2, UART_IT_ORE) != RESET)
			&& (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_ERR) != RESET))
	{
		__HAL_UART_CLEAR_OREFLAG(&huart2);

		LOG_ERROR("UART OVERRUN ERROR\n");
	}

	if ((__HAL_UART_GET_IT(&huart2, UART_IT_RXNE) != RESET)
			&& (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_RXNE) != RESET))
	{

		/* we got a character */
		uint8_t rxData = (uint8_t) (huart2.Instance->RDR & 0xff);
		/* toggle debug led */
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
		/* echo data back to same UART */
		HAL_UART_Transmit(&huart2,&rxData,sizeof(rxData),100);;

		/* Clear RXNE interrupt flag */
		__HAL_UART_SEND_REQ(&huart2, UART_RXDATA_FLUSH_REQUEST);
	}

	/* UART in mode Transmitter ------------------------------------------------*/
	if ((__HAL_UART_GET_IT(&huart2, UART_IT_TXE) != RESET)
			&& (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TXE) != RESET))
	{
		;
	}

	/* UART in mode Transmitter (transmission end) -----------------------------*/
	if ((__HAL_UART_GET_IT(&huart2, UART_IT_TC) != RESET)
			&& (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TC) != RESET))
	{
		;
	}
}
