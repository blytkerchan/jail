#include "contract.h"
#include "registers.h"

void ins_pre_condition(jailvm_registers_t * registers, int valid, jailvm_exception_func func)
{
	if (!valid)
		func(registers);
}

void ins_post_condition(jailvm_registers_t * registers, int valid, jailvm_exception_func func)
{
	if (!valid)
		func(registers);
}

void pre_condition(int valid, jailvm_error_func func)
{
	if (!valid) func();
}

void post_condition(int valid, jailvm_error_func func)
{
	if (!valid) func();
}

