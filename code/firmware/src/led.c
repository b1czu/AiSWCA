#include "cli.h"
#include "led.h"
#include "stm32f070x6.h"
#include "stm32f0xx_hal.h"

/*---- Define function -----------------------------------------------*/

/*---- Static variable -----------------------------------------------*/

/*---- Static function declaration -----------------------------------*/

static void led_hw_init(void);
static void led_logic_init(void);

/*---- Function definition --------------------------------------------*/


void led_init(void)
{
	LOG_INFO("before LED support initialized");
	led_hw_init();
	led_logic_init();
	LOG_INFO("LED support initialized");
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
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);
  
}

static void led_logic_init(void)
{

}

/*---- IRQ ------------------------------------------------------------*/
