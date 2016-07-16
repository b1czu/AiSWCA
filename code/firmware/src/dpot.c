#include "dpot.h"	

/*---- Define function -----------------------------------------------*/

#define dpot_enable()             {DPOT_ENABLE_GPIO_PORT->BSRR=          DPOT_ENABLE_PIN; }
#define dpot_disable()            {DPOT_ENABLE_GPIO_PORT->BSRR=(uint32_t)DPOT_ENABLE_PIN<<16; }

/*---- Static variable -----------------------------------------------*/

static SPI_HandleTypeDef hspi1;

/*---- Static function declaration -----------------------------------*/

static void dpot_hw_init(void);
static void dpot_logic_init(void);

/*---- Function definition --------------------------------------------*/

void dpot_init(void)
{
	dpot_hw_init();
	dpot_logic_init();
}

static void dpot_hw_init(void)
{
  /* SPI1 INIT */
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    #warning TODO
    err_handler(__FILE__,__LINE__,"BUG INFO");
  }

  /* GPIO INIT - Digital resistor enable */
  GPIO_InitTypeDef GPIO_InitStruct;
  HAL_GPIO_WritePin(DPOT_ENABLE_GPIO_PORT, DPOT_ENABLE_PIN, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = DPOT_ENABLE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DPOT_ENABLE_GPIO_PORT, &GPIO_InitStruct);
}

static void dpot_logic_init(void)
{

}


/*---- IRQ ------------------------------------------------------------*/
