/*
 * Salt.h
 *
 * Created: 25/Nov/12 6:30:32 PM
 *  Author: Waqas
 */ 


#ifndef SALT_H_
#define SALT_H_

#include "compiler.h"

typedef struct
{
	unsigned long int index[8];
} uint256_t;


extern volatile uint32_t pass_code [8];
extern volatile bool device_unlocked;
extern volatile uint8_t passcode_byte_index;

extern volatile uint32_t var_W;
extern volatile uint32_t var_W_ticks;

uint32_t random_lcg (void);
void calculate_salt(void);
void XOR_256(uint32_t *value1, uint32_t *value2);

#endif /* SALT_H_ */