#include "contract.h"
#include "registers.h"

void ins_pre_condition(jailvm_registers_t * registers, int valid)
{
	if (valid)
	{ /* pre-condition met - nothing to do */ }
	else
	{	/* pre-condition not met - launch the exception */
	}
}
