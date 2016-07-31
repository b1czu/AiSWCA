#include "adc.h"
#include "main.h"

/*---- Define function -----------------------------------------------*/

#define RANGE_12BITS                   ((uint32_t) 4095)    /* Max digital value with a full range of 12 bits */

/* ADC parameters */
#define ADCCONVERTEDVALUES_BUFFER_SIZE ((uint32_t)  1)    /* Size of array containing ADC converted values */

/*---- Static variable -----------------------------------------------*/

/* ADC handler declaration */
ADC_HandleTypeDef hadc;

/* Variable containing ADC conversions results */
__IO uint16_t   adc_convert_value_buff[ADCCONVERTEDVALUES_BUFFER_SIZE];


/*---- Static function declaration -----------------------------------*/

static void adc_hw_init(void);
static void adc_logic_init(void);

/*---- Function definition --------------------------------------------*/

void adc_init(void){
	adc_hw_init();
	adc_logic_init();
}


static void adc_hw_init(void)
{
  ADC_ChannelConfTypeDef sConfig;
  ADC_AnalogWDGConfTypeDef AnalogWDGConfig;

  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution            = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;    /* Sequencer will convert the number of channels configured below, successively from the lowest to the highest channel number */
  hadc.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait      = DISABLE;
  hadc.Init.LowPowerAutoPowerOff  = DISABLE;
  hadc.Init.ContinuousConvMode    = ENABLE;                        /* Continuous mode to have maximum conversion speed (no delay between conversions) */
  hadc.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
  hadc.Init.ExternalTrigConv      = ADC_SOFTWARE_START;            /* Software start to trig the 1st conversion manually, without external event */
  hadc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Parameter discarded because trig of conversion by software start (no external event) */
  hadc.Init.DMAContinuousRequests = ENABLE;                        /* ADC-DMA continuous requests to match with DMA configured in circular mode */
  hadc.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;
  hadc.Init.SamplingTimeCommon    = ADC_SAMPLETIME_41CYCLES_5;

  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    LOG_ERROR("ADC INIT != HAL_OK");
  }

  /* Configure for the selected ADC regular channel to be converted. */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;

  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    LOG_ERROR("ADC_CONFIG_CHANNEL != HAL_OK");
  }

  /* Set analog watchdog thresholds in order to be between steps of DAC       */
  /* voltage.                                                                 */
  /*  - High threshold:  (RANGE_12BITS * 1/8)                                 */
  /*  - Low threshold:   0                                                    */

  /* Analog watchdog 1 configuration */
  AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_ALL_REG;
  AnalogWDGConfig.Channel = ADC_CHANNEL_1;
  AnalogWDGConfig.ITMode = ENABLE;
  AnalogWDGConfig.HighThreshold = (RANGE_12BITS * 1/8);
  AnalogWDGConfig.LowThreshold = (0);

  if (HAL_ADC_AnalogWDGConfig(&hadc, &AnalogWDGConfig) != HAL_OK)
  {
    /* Channel Configuration Error */
     LOG_ERROR("HAL_ADC_AnalogWDGConfig != HAL_OK");
  }

    /* Run the ADC calibration */
  if (HAL_ADCEx_Calibration_Start(&hadc) != HAL_OK)
  {
    /* Calibration Error */
    LOG_ERROR("HAL_ADCEx_Calibration_Start != HAL_OK");
  }

    /* Start ADC conversion on regular group with transfer by DMA */
  if (HAL_ADC_Start_DMA(&hadc,
                        (uint32_t *)adc_convert_value_buff,
                        ADCCONVERTEDVALUES_BUFFER_SIZE
                       ) != HAL_OK)
  {
    /* Start Error */
    LOG_ERROR("HAL_ADC_Start_DMA != HAL_OK");
  }

}

static void adc_logic_init(void)
{

}

/*---- ADC Callback ------------------------------------------------------------*/

/**
  * @brief  Conversion complete callback in non blocking mode
  * @param  hadc : ADC handle
  * @note   This example shows a simple way to report end of conversion
  *         and get conversion result. You can add your own implementation.
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  //LOG_DEBUG("ADC_CcplC");
}

/**
  * @brief  Conversion DMA half-transfer callback in non blocking mode 
  * @param  hadc: ADC handle
  * @retval None
  */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
  //LOG_DEBUG("ADC_ChplC");
}

/**
  * @brief  Analog watchdog callback in non blocking mode. 
  * @param  hadc: ADC handle
  * @retval None
  */
  void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
  /* Set variable to report analog watchdog out of window status to main      */
  /* program.                                                                 */
  static int i;
  LOG_DEBUG("%d ADC %d", i++,(int) adc_convert_value_buff[0]);
}

/**
  * @brief  ADC error callback in non blocking mode
  *        (ADC conversion with interruption or transfer by DMA)
  * @param  hadc: ADC handle
  * @retval None
  */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
  /* In case of ADC error, call main error handler */
  LOG_ERROR("HAL_ADC_ErrorCallback");
}


/*---- IRQ ------------------------------------------------------------*/

/**
  * @brief  This function handles ADC interrupt request.
  * @param  None
  * @retval None
  */
void ADC1_IRQHandler(void)
{
  HAL_ADC_IRQHandler(&hadc);
}

/**
* @brief  This function handles DMA interrupt request.
* @param  None
* @retval None
*/
void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(hadc.DMA_Handle);
}
