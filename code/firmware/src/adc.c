#include "adc.h"
#include "cli.h"
#include "stm32f070x6.h"
#include "stm32f0xx_hal.h"

/*---- Variable ------------------------------------------------------*/

typedef enum
{
  ADC_DATA_STATE_WAIT_FOR_NEW_DATA = 0,
  ADC_DATA_STATE_NEW_DATA,
}adc_data_state_e;

typedef struct adc_logic_s{
  uint16_t data;
  adc_data_state_e data_state;
}adc_logic_t;

/*---- Define function -----------------------------------------------*/

#define RANGE_12BITS                   ((uint32_t) 4095)    /* Max digital value with a full range of 12 bits */

/* ADC parameters */
#define ADCCONVERTEDVALUES_BUFFER_SIZE ((uint32_t)  1)    /* Size of array containing ADC converted values */

/* TIMER3 parameters */
#define TIMER_FREQUENCY                ((uint32_t)   10)    /* Timer frequency (unit: Hz). With a timer 16 bits and time base freq min 1Hz, range is min=1Hz, max=32kHz. */
#define TIMER_FREQUENCY_RANGE_MIN      ((uint32_t)    1)    /* Timer minimum frequency (unit: Hz), used to calculate frequency range. With a timer 16 bits, maximum frequency will be 32000 times this value. */
#define TIMER_PRESCALER_MAX_VALUE      (0xFFFF-1)           /* Timer prescaler maximum value (0xFFFF for a timer 16 bits) */

/*---- Static variable -----------------------------------------------*/

/* ADC handler declaration */
ADC_HandleTypeDef adch;

/* TIM handler declaration */
TIM_HandleTypeDef    timh;

/* Variable containing ADC conversions results */
__IO uint16_t   adc_convert_value_buff[ADCCONVERTEDVALUES_BUFFER_SIZE];

/* Struct containing ADC logic */
static adc_logic_t adc_logic_container;


/*---- Static function declaration -----------------------------------*/

static void adc_tim_config(void);
static void adc_hw_init(void);
static void adc_logic_init(void);

/*---- Function definition --------------------------------------------*/

void adc_init(void){
	adc_tim_config();
	adc_logic_init();
	adc_hw_init();
	LOG_INFO("KBUS support initialized");
}


static void adc_tim_config(void)
{
  TIM_MasterConfigTypeDef master_timer_config;
  RCC_ClkInitTypeDef clk_init_struct = {0};       /* Temporary variable to retrieve RCC clock configuration */
  uint32_t latency;                               /* Temporary variable to retrieve Flash Latency */

  uint32_t timer_clock_frequency = 0;             /* Timer clock frequency */
  uint32_t timer_prescaler = 0;                   /* Time base prescaler to have timebase aligned on minimum frequency possible */

  /* Configuration of timer as time base:                                     */
  /* Caution: Computation of frequency is done for a timer instance on APB1   */
  /*          (clocked by PCLK1)                                              */
  /* Timer frequency is configured from the following constants:              */
  /* - TIMER_FREQUENCY: timer frequency (unit: Hz).                           */
  /* - TIMER_FREQUENCY_RANGE_MIN: timer minimum frequency possible            */
  /*   (unit: Hz).                                                            */
  /* Note: Refer to comments at these literals definition for more details.   */

  /* Retrieve timer clock source frequency */
  HAL_RCC_GetClockConfig(&clk_init_struct, &latency);
  /* If APB1 prescaler is different of 1, timers have a factor x2 on their    */
  /* clock source.                                                            */
  if (clk_init_struct.APB1CLKDivider == RCC_HCLK_DIV1)
  {
    timer_clock_frequency = HAL_RCC_GetPCLK1Freq();
  }
  else
  {
    timer_clock_frequency = HAL_RCC_GetPCLK1Freq() *2;
  }

  /* Timer prescaler calculation */
  /* (computation for timer 16 bits, additional + 1 to round the prescaler up) */
  timer_prescaler = (timer_clock_frequency / (TIMER_PRESCALER_MAX_VALUE * TIMER_FREQUENCY_RANGE_MIN)) +1;

  /* Set timer instance */
  timh.Instance = TIM3;

  /* Configure timer parameters */
  timh.Init.Period            = ((timer_clock_frequency / (timer_prescaler * TIMER_FREQUENCY)) - 1);
  timh.Init.Prescaler         = (timer_prescaler - 1);
  timh.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  timh.Init.CounterMode       = TIM_COUNTERMODE_UP;
  timh.Init.RepetitionCounter = 0x0;

  if (HAL_TIM_Base_Init(&timh) != HAL_OK)
  {
    /* Timer initialization Error */
    LOG_ERROR("HAL_TIM_Base_Init != HAL_OK");
  }

  /* Timer TRGO selection */
  master_timer_config.MasterOutputTrigger = TIM_TRGO_UPDATE;
  master_timer_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  if (HAL_TIMEx_MasterConfigSynchronization(&timh, &master_timer_config) != HAL_OK)
  {
    /* Timer TRGO selection Error */
    LOG_ERROR("HAL_TIMEx_MasterConfigSynchronization != HAL_OK");
  }

  /* Timer enable */
  if (HAL_TIM_Base_Start(&timh) != HAL_OK)
  {
    /* Counter Enable Error */
    LOG_ERROR("HAL_TIM_Base_Start != HAL_OK");
  }
}

static void adc_hw_init(void)
{

  ADC_ChannelConfTypeDef sConfig;
  ADC_AnalogWDGConfTypeDef AnalogWDGConfig;

  adch.Instance = ADC1;
  adch.Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV1;
  adch.Init.Resolution            = ADC_RESOLUTION_12B;
  adch.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  adch.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;    /* Sequencer will convert the number of channels configured below, successively from the lowest to the highest channel number */
  adch.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
  adch.Init.LowPowerAutoWait      = DISABLE;
  adch.Init.LowPowerAutoPowerOff  = DISABLE;
  adch.Init.ContinuousConvMode    = DISABLE;                        /* Continuous mode to have maximum conversion speed (no delay between conversions) */
  adch.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
  adch.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T3_TRGO;  /* Trig of conversion start done by external event */
  adch.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_RISING;
  adch.Init.DMAContinuousRequests = ENABLE;                        /* ADC-DMA continuous requests to match with DMA configured in circular mode */
  adch.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;
  adch.Init.SamplingTimeCommon    = ADC_SAMPLETIME_41CYCLES_5;

  if (HAL_ADC_Init(&adch) != HAL_OK)
  {
    LOG_ERROR("ADC INIT != HAL_OK");
  }

  /* Configure for the selected ADC regular channel to be converted. */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;

  if (HAL_ADC_ConfigChannel(&adch, &sConfig) != HAL_OK)
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

  if (HAL_ADC_AnalogWDGConfig(&adch, &AnalogWDGConfig) != HAL_OK)
  {
    /* Channel Configuration Error */
     LOG_ERROR("HAL_ADC_AnalogWDGConfig != HAL_OK");
  }

    /* Run the ADC calibration */
  if (HAL_ADCEx_Calibration_Start(&adch) != HAL_OK)
  {
    /* Calibration Error */
    LOG_ERROR("HAL_ADCEx_Calibration_Start != HAL_OK");
  }

    /* Start ADC conversion on regular group with transfer by DMA */
  if (HAL_ADC_Start_DMA(&adch,
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
  adc_logic_container.data = 0;
  adc_logic_container.data_state = ADC_DATA_STATE_WAIT_FOR_NEW_DATA;
}

void adc_logic(void)
{
	/* new data, it's time to process it and set dpot on correct value */
	if(adc_logic_container.data_state == ADC_DATA_STATE_NEW_DATA){
		LOG_DEBUG("adc %d",adc_logic_container.data);
		adc_logic_container.data_state = ADC_DATA_STATE_WAIT_FOR_NEW_DATA;

		// dpot set data

	}
	/* button is released, disconnect dpot */
	else{
		// dpot off

	}
}

/*---- ADC Callback ------------------------------------------------------------*/

/**
  * @brief  Conversion complete callback in non blocking mode
  * @param  adch : ADC handle
  * @note   This example shows a simple way to report end of conversion
  *         and get conversion result. You can add your own implementation.
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *adch)
{
  //LOG_DEBUG("ADC_CcplC");
}

/**
  * @brief  Conversion DMA half-transfer callback in non blocking mode 
  * @param  adch: ADC handle
  * @retval None
  */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* adch)
{
  //LOG_DEBUG("ADC_ChplC");
}

/**
  * @brief  Analog watchdog callback in non blocking mode. 
  * @param  adch: ADC handle
  * @retval None
  */
  void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* adch)
{
  /* Set variable to report analog watchdog out of window status to main      */
  /* program.                                                                 */

  //DEBUG
  //static int i;
  //LOG_DEBUG("%d ADC %d", i++,(int) adc_convert_value_buff[0]);

  //get data from buff and set
  adc_logic_container.data = adc_convert_value_buff[0];
  adc_logic_container.data_state = ADC_DATA_STATE_NEW_DATA;

}

/**
  * @brief  ADC error callback in non blocking mode
  *        (ADC conversion with interruption or transfer by DMA)
  * @param  adch: ADC handle
  * @retval None
  */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *adch)
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
  HAL_ADC_IRQHandler(&adch);
}

/**
* @brief  This function handles DMA interrupt request.
* @param  None
* @retval None
*/
void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(adch.DMA_Handle);
}
