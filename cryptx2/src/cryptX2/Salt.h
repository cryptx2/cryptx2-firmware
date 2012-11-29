/*
 * Salt.h
 *
 * Created: 25/Nov/12 6:30:32 PM
 *  Author: Waqas
 */ 


#ifndef SALT_H_
#define SALT_H_

#include <string.h>
#include "compiler.h"


#define SALT_PAGE_NUMBER		100			// it can be from 1-512 for 256 KBytes of Flash
#define SALT_MEM_ADDRESS		0x80000000 + (512L * SALT_PAGE_NUMBER)
#define SALT_STRUCT				(*((volatile salt_t *)SALT_MEM_ADDRESS)))

typedef struct 
{
	unsigned long int salt_value_primary[8];
	unsigned long int crc_primary;
	unsigned long int reserved[110];
	unsigned long int salt_value_secondary[8];
	unsigned long int crc_secondary;	
} salt_t;

typedef struct
{
	unsigned long int index[8];
} uint256_t;


extern volatile uint32_t pass_code [8];
extern volatile bool device_unlocked;
extern volatile uint8_t passcode_byte_index;

extern volatile uint32_t var_W;
extern volatile uint32_t var_W_ticks;
extern volatile uint256_t var_Hkey;
extern volatile uint256_t var_R;
extern volatile uint256_t var_T;
extern volatile uint256_t var_Salt;

uint32_t random_lcg (void);
void calculate_salt(void);
void xor_func (uint32_t *value1, uint32_t *value2, uint8_t len);
void Start_W_timer(void);

#endif /* SALT_H_ */