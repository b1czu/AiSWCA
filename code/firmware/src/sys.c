#include "firmware_cfg.h"
#include "led.h"
#include "printf.h"
#include "sys.h"
#include "stm32f070x6.h"
#include "stm32f0xx_hal.h"

/*---- Define function -----------------------------------------------*/

/*---- Static variable -----------------------------------------------*/

static uint64_t sys_time;

/*---- Static function declaration -----------------------------------*/

/*---- Function definition --------------------------------------------*/

uint64_t sys_get_time(void)
{
	return sys_time;
}

void sys_print_info(void)
{
	printf_("AiSWCA v%s %s %s\n\r",FW_VERSION,__DATE__,__TIME__);
	printf_("build %s\n\r",BUILD);
	printf_("HW v%s\n\r", HW_VERSION);	
}

/*---- IRQ ------------------------------------------------------------*/

/* SYSTEM IRQ */
void SysTick_Handler(void)
{
	/* LED task */
	//static uint64_t i = 0;
	if(sys_time % 1000 == 0){
		led_1Toggle();
		//LOG_INFO("%s %d","TEST",(int)i++ );
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
