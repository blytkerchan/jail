#ifndef jailvm_exception_h
#define jailvm_exception_h

#include <stdint.h>
#include <setjmp.h>

jmp_buf exception_jump;

#define JAILVM_EXCEPTION_INVALID_OPCODE		0x00000001
#define JAILVM_EXCEPTION_INVALID_OPPARAM	0x00000002

extern uint32_t jailvm_exception_handlers[];

void jailvm_exception_handlers_init(void);

typedef struct jailvm_registers_type jailvm_registers_t;

void jailvm_invalid_opcode(jailvm_registers_t * registers);
void jailvm_invalid_opparam(jailvm_registers_t * registers);

#endif

