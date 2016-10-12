#ifndef __ADC_H
#define __ADC_H

/*

ADC peripheral to perform conversions with analog watchdog and out-of-window interruptions enabled.

ADC works in external trigger mode. ADC is operating in not continuous mode and conversions are trigger by external trigger: timer3.
GPIO peripheral is used in analog mode to drive signal from device pin to ADC input (ADC1 channel 1).Timer peripheral is used to trigger ADC conversions.DMA peripheral is used to transfer ADC converted data.

ADC settings:
 - Regular group:
   Conversions are triggered by external event (timer at 100Hz).
 - Continuous mode is disabled (and sequencer disabled: only 1 channel selected) to yield only 1 conversion at each conversion trigger.
 - Analog watchdog 1 is enabled, minimum and maximum thresholds are respectively set 
   to 0 and 1/8 of full range scale (between 0V and 0.41V with full range of 3.3V).

ADC conversion results:
 - ADC regular conversions results are transferred automatically by DMA, into variable "adc_convert_value_buff".
 - DMA and ADC are configured to operate continuously, in circular mode.
   When DMA transfer half-buffer and full buffer lengths are reached, callbacks 
   HAL_ADC_ConvHalfCpltCallback() and HAL_ADC_ConvCpltCallback() are called.

Board settings:
 - ADC is configured to convert ADC_CHANNEL_1 (pin PA.01).

 Watchdog OUT-OF-WINDOW:

								  __		__ 3.3V
								 /			|
								/			|
HAL_ADC_LevelOutOfWindowCallback			|
								\			|
								 \__		|__ 1/8 Vcc
											|
									   		|__ 0V

 */

void adc_init(void);
void adc_logic(void);

void ADC1_IRQHandler(void);
void DMA1_Channel1_IRQHandler(void);

#endif /* __ADC_H */
