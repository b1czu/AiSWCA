#ifndef __SYS_H
#define __SYS_H

#include "main.h"

uint64_t sys_get_time(void);

void SysTick_Handler(void);
void HardFault_Handler(void);

#endif /* __CLI_H */