#ifndef __HW_BOARD_H
#define __HW_BOARD_H

#include "main.h"

#define LED1_GPIO_PORT        GPIOA
#define LED1_GPIO_PIN         GPIO_PIN_4
#define led1On()               {LED1_GPIO_PORT->BSRR=          LED1_GPIO_PIN; }
#define led1Off()              {LED1_GPIO_PORT->BSRR=(uint32_t)LED1_GPIO_PIN<<16; }
#define led1Toggle()           {LED1_GPIO_PORT->ODR ^=         LED1_GPIO_PIN; }

void hw_gpio_cfg(void);

#endif /* __MAIN_H */