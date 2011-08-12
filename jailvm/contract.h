#ifndef jailvm_contract_h
#define jailvm_contract_h

typedef struct jailvm_registers_type jailvm_registers_t;

typedef void (*jailvm_exception_func)(jailvm_registers_t*);
/* Instruction pre-condition */
void ins_pre_condition(jailvm_registers_t * registers, int valid, jailvm_exception_func func);
/* Instruction post-condition */
void ins_post_condition(jailvm_registers_t * registers, int valid, jailvm_exception_func func);

typedef void (*jailvm_error_func)(void);
/* pre-condition */
void pre_condition(int valid, jailvm_error_func func);
/* post-condition */
void post_condition(int valid, jailvm_error_func func);

#endif

