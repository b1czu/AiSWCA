#include "led.h"

/*---- Define function -----------------------------------------------*/

/*---- Static variable -----------------------------------------------*/

/*---- Static function declaration -----------------------------------*/

static void led_hw_init(void);
static void led_logic_init(void);

/*---- Function definition --------------------------------------------*/


void led_init(void)
{
	led_hw_init();
	led_logic_init();
}

static void led_hw_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /*          LED1           */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = LED1_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);
  
}

static void led_logic_init(void)
{

}

/*---- IRQ ------------------------------------------------------------*/
