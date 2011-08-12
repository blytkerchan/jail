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
	fprintf(stderr, "Invalid opcode at address 0x%.8x\n", registers->ip);
	registers->eip = exception_handlers[JAILVM_EXCEPTION_INVALID_OPCODE];
	longjmp(registers->jailvm_exception_jump, 1);
}

void jailvm_invalid_opparam(jailvm_registers_t * registers)
{
	fprintf(stderr, "Invalid opparam at address 0x%.8x\n", registers->ip + 1);
	registers->eip = jailvm_exception_handlers[JAILVM_EXCEPTION_INVALID_OPPARAM];
	longjmp(registers->jailvm_exception_jump, 1);
}

void jailvm_integer_underflow(jailvm_registers_t * registers)
{
	fprintf(stderr, "Integer underflow exception at address 0x%.8x\n", registers->ip);
	registers->eip = jailvm_exception_handlers[JAILVM_EXCEPTION_INTEGER_UNDERFLOW];
	longjmp(registers->jailvm_exception_jump, 1);
}

void jailvm_integer_overflow(jailvm_registers_t * registers)
{
	fprintf(stderr, "Integer overflow exception at address 0x%.8x\n", registers->ip);
	registers->eip = jailvm_exception_handlers[JAILVM_EXCEPTION_INTEGER_OVERFLOW];
	longjmp(registers->jailvm_exception_jump, 1);
}

void jailvm_access_violation(jailvm_registers_t * registers)
{
	fprintf(stderr, "Access violation exception at address 0x%.8x\n", registers->ip);
	registers->eip = jailvm_exception_handlers[JAILVM_EXCEPTION_ACCESS_VIOLATION];
	longjmp(registers->jailvm_exception_jump, 1);
}

void jailvm_bad_alloc(void)
{
	fprintf(stderr, "Fatal memory allocation error\n");
	exit(1);
}

void jailvm_stacked_exception1(jailvm_registers_t * registers)
{
	jailvm_stacked_exception();
}

void jailvm_stacked_exception(void)
{
	fprintf(stderr, "Exception while handling an exception\n");
	exit(1);
}

void jailvm_unexpected_eof(void)
{
	fprintf(stderr, "Unexpected end of file\n");
	exit(1);
}

void jailvm_unhandled_exception(void)
{
	fprintf(stderr, "Unhandled exception\n");
	exit(1);
}

