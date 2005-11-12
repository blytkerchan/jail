#ifndef jailvm_instructions_h
#define jailvm_instructions_h

typedef struct jailvm_registers_type jailvm_registers_t;

typedef void (*jailvm_instruction_func)(jailvm_registers_t *, uint8_t);
extern jailvm_instruction_func jailvm_instructions[];

#endif

