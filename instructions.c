#include "instructions.h"
#include "registers.h"

static void Nop(jailvm_registers_t * registers, uint8_t param)
{ /* no-op */ }

static void Add(jailvm_registers_t * registers, uint8_t param)
{
	ins_pre_condition(registers, param == 0 || param == 1, jailvm_invalid_opparam);
	switch (param)
	{
	case 0 : // unsigned addition
		registers->r[0] = registers->r[1] + registers->r[2];
		break;
	case 1 : // signed addition
	{
		int32_t r1 = (int32_t)registers->r[1];
		int32_t r2 = (int32_t)registers->r[2];
		registers->r[0] = (uint32_t)(r1 + r2);
	}
	}
}

jailvm_instruction_func jailvm_instructions[] = {
	Nop,
};
