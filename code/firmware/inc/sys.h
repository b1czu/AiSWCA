#ifndef __SYS_H
#define __SYS_H

#include <stdint.h>

void sys_print_info(void);

uint64_t sys_get_time(void);

void SysTick_Handler(void);

void HardFault_Handler(void);

#endif /* __CLI_H */
