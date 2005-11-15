#ifndef jailvm_registers_h
#define jailvm_registers_h

#include <stdint.h>

typedef struct jailvm_object_type jailvm_object_t;
struct jailvm_registers_type
{
	uint32_t r[8];			// general-purpose registers
	uint16_t o[4];			// object registers
//	uint32_t a[4];			// address registers.
//	As these aren't really needed for the internal state,
//	we don't actually keep them - we use _t for just about
//	everything in stead.
	jailvm_object_t * _o[4];	// base address of the object pointed to by o[n] is in _o[n]
	uint8_t* _t[4];			// whatever is loaded in (o[n] + a[n]) is pointer to by _t[n]
	uint32_t ip;			// instruction pointer
	uint32_t eip;			// exception instruction pointer
};

#endif

