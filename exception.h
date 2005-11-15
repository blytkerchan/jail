#ifndef jailvm_exception_h
#define jailvm_exception_h

#include <stdint.h>
#include <setjmp.h>

jmp_buf jailvm_exception_jump;

#define JAILVM_EXCEPTION_INVALID_OPCODE		0x00000001
#define JAILVM_EXCEPTION_INVALID_OPPARAM	0x00000002
#define JAILVM_EXCEPTION_INTEGER_UNDERFLOW	0x00000003
#define JAILVM_EXCEPTION_INTEGER_OVERFLOW	0x00000004
#define JAILVM_EXCEPTION_ACCESS_VIOLATION	0x00000005

extern uint32_t jailvm_exception_handlers[];

void jailvm_exception_handlers_init(void);

typedef struct jailvm_registers_type jailvm_registers_t;

void jailvm_invalid_opcode(jailvm_registers_t * registers);
void jailvm_invalid_opparam(jailvm_registers_t * registers);
void jailvm_integer_underflow(jailvm_registers_t * registers);
void jailvm_integer_overflow(jailvm_registers_t * registers);
void jailvm_access_violation(jailvm_registers_t * registers);

void jailvm_bad_alloc(void);

#endif

