#ifndef jailvm_registers_h
#define jailvm_registers_h

#include <stdint.h>

struct jailvm_registers_type
{
	uint32_t r[8];	// general-purpose registers
	uint16_t o[4];	// object registers
	uint32_t a[4];	// address registers
	uint32_t eip;
};

#endif

