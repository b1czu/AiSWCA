#include "error.h"
#include "cli.h"

/*---- Define function -----------------------------------------------*/

/*---- Static variable -----------------------------------------------*/

/*---- Static function declaration -----------------------------------*/

/*---- Function definition --------------------------------------------*/

#warning TODO
void err_handler(char *file,uint32_t line,char* info)
{
	LOG_ERROR("%s %d %s", file, line, info );
}

/*---- IRQ ------------------------------------------------------------*/


