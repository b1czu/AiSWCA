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
  huart2.Init.BaudRate = 9600;
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

/*---- IRQ ------------------------------------------------------------*/

void USART2_IRQHandler(void){
  uint8_t rxData[1];
  uint32_t irq_Status = USART2->ISR;
  if(irq_Status&0x0F)
  {
    #warning ADD ERROR CODE
  }
  else
  {
    if(irq_Status & UART_FLAG_RXNE)       // read interrupt
    {                  
      rxData[0] = (uint8_t) USART2->RDR;
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
      HAL_UART_Transmit(&huart2,&rxData[0],sizeof(rxData),100);
    }
    else if(irq_Status & UART_FLAG_TXE)
    {

    }
  } 
}