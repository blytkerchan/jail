#ifndef jailvm_contract_h
#define jailvm_contract_h

typedef struct jailvm_registers_type jailvm_registers_t;

/* Instruction pre-condition */
void ins_pre_condition(jailvm_registers_t * registers, int valid);
/* Instruction post-condition */
void ins_post_condition(jailvm_registers_t * registers, int valid);

#endif

