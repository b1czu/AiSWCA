#ifndef __LED_H
#define __LED_H

#define LED1_GPIO_PORT       	GPIOA
#define LED1_GPIO_PIN         	GPIO_PIN_4

#define led_1On()               	{LED1_GPIO_PORT->BSRR=          LED1_GPIO_PIN; }
#define led_1Off()              	{LED1_GPIO_PORT->BSRR=(uint32_t)LED1_GPIO_PIN<<16; }
#define led_1Toggle()           	{LED1_GPIO_PORT->ODR ^=         LED1_GPIO_PIN; }

void led_init(void);

#endif /* __LED_H */
