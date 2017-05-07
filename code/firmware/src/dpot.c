#include "cli.h"
#include "dpot.h"
#include "error.h"
#include "stm32f070x6.h"
#include "stm32f0xx_hal.h"

/*---- Define function -----------------------------------------------*/

#define DPOT_ENABLE()             {DPOT_ENABLE_GPIO_PORT->BSRR=          DPOT_ENABLE_PIN; }
#define DPOT_DISABLE()            {DPOT_ENABLE_GPIO_PORT->BSRR=(uint32_t)DPOT_ENABLE_PIN<<16; }

/*---- Static variable -----------------------------------------------*/
static int dpot_timer;
static uint8_t dpot_blip_active = 0;
static uint16_t dpot_blip_time = 0;
static SPI_HandleTypeDef hspi1;

/*---- Static function declaration -----------------------------------*/

static void dpot_hw_init(void);
static void dpot_logic_init(void);

/*---- Function definition --------------------------------------------*/
void dpot_enable(void)
{
	DPOT_ENABLE();
}

void dpot_disable(void)
{
	DPOT_DISABLE();
	/* Set pot to maximum resistance (just to be sure)*/
	dpot_set_value(DPOT_ADDRESS_POT_0, 0);
}

void dpot_blip(uint16_t ms)
{
	DPOT_ENABLE();
	dpot_blip_active = 1;
	dpot_blip_time = ms;
}

void dpot_blip_cancel(void)
{
	dpot_blip_active = 0;
	dpot_timer = 0;
}

void dpot_set_value(DPOT_Address Address, uint8_t value)
{
	uint16_t tx_data = 0;

	/* Move address bytes left by 8 positions */
	tx_data = (uint16_t) (Address << 8);

	/* Assign desired resistance value to tx_data */
	tx_data |= value;

	/* Write tx_data to Digital Pot */
	HAL_GPIO_WritePin(DPOT_CS_GPIO_PORT, DPOT_CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, (uint8_t*) &tx_data, 1,10);
	HAL_GPIO_WritePin(DPOT_CS_GPIO_PORT, DPOT_CS_PIN, GPIO_PIN_SET);
}

void dpot_init(void)
{
	dpot_hw_init();
	dpot_logic_init();
	LOG_INFO("DPOT support initialized");
}

void dpot_systick_handler(void) {
	if (dpot_blip_active == 1) {
		dpot_timer++;
	}
	if (dpot_timer > dpot_blip_time) {
		dpot_blip_active = 0;
		dpot_timer = 0;
		dpot_disable();
	}

}

static void dpot_hw_init(void)
{
  /* SPI1 INIT */
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_10BIT;
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

   /* GPIO INIT - Port enable */
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* GPIO INIT - Digital resistor enable */
  HAL_GPIO_WritePin(DPOT_ENABLE_GPIO_PORT, DPOT_ENABLE_PIN, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = DPOT_ENABLE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DPOT_ENABLE_GPIO_PORT, &GPIO_InitStruct);

  /* GPIO INIT - Digital resistor CS */
  HAL_GPIO_WritePin(DPOT_CS_GPIO_PORT, DPOT_CS_PIN, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = DPOT_CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DPOT_CS_GPIO_PORT, &GPIO_InitStruct);
}

static void dpot_logic_init(void)
{	dpot_timer = 0;
	/* Set pot state to disabled (infinite resistance) */
	dpot_disable();
	/* Set pot to maximum resistance */
	dpot_set_value(DPOT_ADDRESS_POT_0, 0);
}


/*---- IRQ ------------------------------------------------------------*/
