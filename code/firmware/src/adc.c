#include "adc.h"
#include "main.h"

/*---- Define function -----------------------------------------------*/

/*---- Static variable -----------------------------------------------*/

ADC_HandleTypeDef hadc;

/*---- Static function declaration -----------------------------------*/

static void adc_hw_init(void);
static void adc_logic_init(void);

/*---- Function definition --------------------------------------------*/

void adc_init(void){
	adc_hw_init();
	adc_logic_init();
}


/* USART2 init function */
static void adc_hw_init(void)
{
  ADC_ChannelConfTypeDef sConfig;

  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    LOG_ERROR("ADC INIT != HAL_OK");
  }

    /**Configure for the selected ADC regular channel to be converted. 
    */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    LOG_ERROR("ADC_CONFIG_CHANNEL != HAL_OK");
  }
}

static void adc_logic_init(void)
{

}

uint32_t adc_get_data(void)
{
	uint32_t adc_result = 0;

	LOG_INFO("ADC_START %s",(HAL_ADC_Start(&hadc) == HAL_OK) ? "OK":"ERROR");
	LOG_INFO("ADC_PollForConversion %s",(HAL_ADC_PollForConversion(&hadc,500) == HAL_OK) ? "OK":"ERROR");
	adc_result = HAL_ADC_GetValue(&hadc);
	LOG_INFO("time %d Tem %d ", sys_get_time(),adc_result);
	LOG_INFO("ADC_STOP %s",(HAL_ADC_Stop(&hadc) == HAL_OK) ? "OK":"ERROR");

	

	return adc_result;
}


/*---- IRQ ------------------------------------------------------------*/