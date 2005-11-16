#ifndef jailvm_instructions_h
#define jailvm_instructions_h

typedef struct jailvm_registers_type jailvm_registers_t;

typedef void (*jailvm_instruction_func)(jailvm_registers_t *, uint8_t);

void jailvm_instruction_execute(jailvm_registers_t * registers, uint8_t opcode, uint8_t param);
#endif

