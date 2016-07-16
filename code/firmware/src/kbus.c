#include "kbus.h"
#include "main.h"

/*---- Define function ------------------------------------------------*/

/*---- Static variable ------------------------------------------------*/

static UART_HandleTypeDef huart1;

/*---- Static function declaration ------------------------------------*/

/*---- Function definition --------------------------------------------*/

/* USART1 init function */
void kbus_init(void)
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

/*---- IRQ ------------------------------------------------------------*/

void USART1_IRQHandler(void)
{
  uint8_t rxData[1];
  uint32_t irq_Status = USART1->ISR;
  if(irq_Status&0x0F)
  {
    #warning ADD ERROR CODE
  }
  else
  {
    if(irq_Status & UART_FLAG_RXNE)
    {                  
      rxData[0] = (uint8_t) USART1->RDR;
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
      HAL_UART_Transmit(&huart1,&rxData[0],sizeof(rxData),100);
    }
    else if(irq_Status & UART_FLAG_TXE)
    {

    }
  } 
}
