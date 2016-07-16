#include "sys.h"

/*---- Define function -----------------------------------------------*/

/*---- Static variable -----------------------------------------------*/

static uint64_t sys_time;

/*---- Static function declaration -----------------------------------*/

/*---- Function definition --------------------------------------------*/

uint64_t sys_get_time(void)
{
	return sys_time;
}

/*---- IRQ ------------------------------------------------------------*/

/* SYSTEM IRQ */
void SysTick_Handler(void)
{
  /* LED task */
  if(sys_time % 1000 == 0){
   	led_1Toggle();
  }
  
  /* Next task */	
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();

  /* Increment system time */
  sys_time++;
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  	led_1On();
  }
}
