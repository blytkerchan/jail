#include "exception.h"
#include <stdio.h>
#include "registers.h"

uint32_t jailvm_exception_handlers[255];

void jailvm_exception_handlers_init(void)
{
	int i;
	
	for (i = 0; i < sizeof(jailvm_exception_handlers) / sizeof(uint32_t); ++i)
		jailvm_exception_handlers[i] = 0;
}

void jailvm_invalid_opcode(jailvm_registers_t * registers)
{
	fprintf(stderr, "Invalid opcode at address 0x%.8x\n", registers->eip);
	registers->eip = exception_handlers[JAILVM_EXCEPTION_INVALID_OPCODE];
	longjmp(exception_jump, 1);
}

void jailvm_invalid_opparam(jailvm_registers_t * registers)
{
	fprintf(stderr, "Invalid opparam at address 0x%.8x\n", registers->eip + 1);
	registers->eip = jailvm_exception_handlers[JAILVM_EXCEPTION_INVALID_OPPARAM];
	longjmp(exception_jump, 1);
}
