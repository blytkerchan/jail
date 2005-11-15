#include "instructions.h"
#include "registers.h"

/* No-op instruction: doesn't do anything (opcode 0) */
static void Nop(jailvm_registers_t * registers, uint8_t param)
{ /* no-op */ }

/* Add instruction: adds r1 to r2 and saves the result in r0. According to
 * the parameter of the operation, the addition is either signed
 * (param == 1) or unsigned (param == 0)
 * Note that we try to catch some cases of integer overflows, but we pobably
 * don't catch all of them... */
static void Add(jailvm_registers_t * registers, uint8_t param)
{
	ins_pre_condition(registers, param == 0 || param == 1, jailvm_invalid_opparam);
	switch (param)
	{
	case 0 : // unsigned addition
		ins_pre_condition(registers, 0xFFFFFFFF - registers->r[2] > registers->r[1], jailvm_integer_overflow);
		ins_pre_condition(registers, 0xFFFFFFFF - registers->r[1] > registers->r[2], jailvm_integer_overflow);
		registers->r[0] = registers->r[1] + registers->r[2];
		break;
	case 1 : // signed addition
	{
		int32_t r1 = (int32_t)registers->r[1];
		int32_t r2 = (int32_t)registers->r[2];
		ins_pre_condition(registers, 0x7FFFFFFF - r2 > r1, jailvm_integer_overflow);
		ins_pre_condition(registers, 0x7FFFFFFF - r1 > r2, jailvm_integer_overflow);
		registers->r[0] = (uint32_t)(r1 + r2);
	}
	}
}

/* Sub instruction: substract r2 from r1 and store the result in r0
 * One case of integer underflow is caught by this function - we
 * should probably catch more of them... */
static void Sub(jailvm_registers_t * registers, uint8_t param)
{
	ins_pre_condition(registers, param == 0 || param == 1, jailvm_invalid_opparam);
	switch (param)
	{
	case 0 : // unsigned substraction
		ins_pre_condition(registers, registers->r[2] > registers->r[1], jailvm_integer_underflow);
		registers->r[0] = registers->r[1] - registers->r[2];
		break;
	case 1 : // signed substraction
	{
		int32_t r1 = (int32_t)registers->r[1];
		int32_t r2 = (int32_t)registers->r[2];
		registers->r[0] = (uint32_t)(r1 - r2);
	}
	}
}

/* Mul instruction: multiply r1 by r2 and store the result in r0 */
static void Mul(jailvm_registers_t * registers, uint8_t param)
{
	ins_pre_condition(registers, param == 0 || param == 1, jailvm_invalid_opparam);
	switch (param)
	{
	case 0 : // unsigned multiply
		registers->r[0] = registers->r[1] * registers->r[2];
		break;
	case 1 : // signed multiply
	{
		int32_t r1 = (int32_t)registers->r[1];
		int32_t r2 = (int32_t)registers->r[2];
		registers->r[0] = (uint32_t)(r1 * r2);
	}
	}
}

/* Div instruction: divide r1 by r2 and store the result in r0 */
static void Div(jailvm_registers_t * registers, uint8_t param)
{
	ins_pre_condition(registers, param == 0 || param == 1, jailvm_invalid_opparam);
	switch (param)
	{
	case 0 : // unsigned divide
		registers->r[0] = registers->r[1] / registers->r[2];
		break;
	case 1 : // signed divide
	{
		int32_t r1 = (int32_t)registers->r[1];
		int32_t r2 = (int32_t)registers->r[2];
		registers->r[0] = (uint32_t)(r1 / r2);
	}
	}
}

/* As of here, all instructions work only on unsigned
 * general-purpose registers */

/* And instruction: AND r1 with r2 and store the result in r0 */
static void And(jailvm_registers_t * registers, uint8_t param)
{
	ins_pre_condition(registers, param == 0, jailvm_invalid_opparam);
	registers->r[0] = registers->r[1] & registers->r[2];
}

/* Or instruction: OR r1 with r2 and store the result in r0 */
static void Or(jailvm_registers_t * registers, uint8_t param)
{
	ins_pre_condition(registers, param == 0, jailvm_invalid_opparam);
	registers->r[0] = registers->r[1] | registers->r[2];
}

/* Xor instruction: XOR r1 with r2 and store the result in r0 */
static void Xor(jailvm_registers_t * registers, uint8_t param)
{
	ins_pre_condition(registers, param == 0, jailvm_invalid_opparam);
	registers->r[0] = registers->r[1] ^ registers->r[2];
}

/* The following instructions are not mathematical */

/* Int instruction - thow an exception according to the parameter */
static void Int(jailvm_registers_t * registers, uint8_t param)
{
	registers->ip += 2; 
	registers->eip = jailvm_exception_handlers[param];
	longjmp(jailvm_exception_jump);
}

/* Load an object into the object register and nullify the corresponding address register */
static void Load(jailvm_registers_t * registers, uint8_t param)
{
	/* general-purpose register to use */
	int gp_reg = (param >> 5) & 7;
	/* object register to use */
	int obj_reg = (param >> 3) & 3;
	ins_pre_condition(registers, param & 7 == 0, jailvm_invalid_opparam);
	registers->o[obj_reg] = registers->r[gp_reg];
	registers->_o[obj_reg] = jailvm_object_get(registers->o[obj_reg]);
	ins_post_condition(registers, registers->_o[obj_reg] != NULL, jailvm_access_violation);
	registers->_t[obj_reg] = registers->_o[obj_reg]->p;
	post_condition(registers->_t[obj_reg] != NULL, jailvm_internal_error);
}

jailvm_instruction_func jailvm_instructions[] = {
	Nop, Add, Sub, Mul, Div, And, Or, Xor, Int, Load
};
