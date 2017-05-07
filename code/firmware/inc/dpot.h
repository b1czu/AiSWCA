#ifndef __DPOT_H
#define __DPOT_H

#include <stdint.h>

#define DPOT_ENABLE_GPIO_PORT			GPIOA
#define DPOT_ENABLE_PIN					GPIO_PIN_0
#define DPOT_CS_GPIO_PORT				GPIOA
#define DPOT_CS_PIN						GPIO_PIN_6


/**
  * @brief  DPOT_ADDRESS DPOT Address enumeration
  */
typedef enum
{
	DPOT_ADDRESS_POT_0 = 0,
	DPOT_ADDRESS_POT_1,
	DPOT_ADDRESS_POT_2,
	DPOT_ADDRESS_POT_3
}DPOT_Address;
/**
  * @}
  */

void dpot_init(void);
void dpot_set_value(DPOT_Address Address, uint8_t value);
void dpot_enable(void);
void dpot_disable(void);
void dpot_blip(void);
void dpot_systick_handler(void);

#endif /* __DPOT_H */
