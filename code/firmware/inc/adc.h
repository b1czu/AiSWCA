#ifndef __ADC_H
#define __ADC_H

#include "main.h"

void adc_init(void);


void ADC1_IRQHandler(void);
void DMA1_Channel1_IRQHandler(void);


#endif /* __ADC_H */