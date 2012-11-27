/*
 * Salt.c
 *
 * Created: 25/Nov/12 6:30:07 PM
 *  Author: Waqas
 */ 

#include "Salt.h"
#include "sha256.h" 


volatile uint32_t pass_code [8] = {0};
volatile bool device_unlocked = false;
volatile uint8_t passcode_byte_index = 0;
volatile unsigned long int rseed = 0;
volatile uint256_t var_Hkey;
volatile uint256_t var_R;
volatile uint256_t var_T;
volatile uint32_t var_W = 0xFFFFFFFF;
volatile uint32_t var_W_ticks = 0;
volatile uint256_t var_Salt;

uint32_t random_lcg (void)
{
	rseed = (rseed * 1103515245 + 12345) & RAND_MAX;
	return rseed;
}

//void random_256 ()

void calculate_salt(void)
{
	uint32_t random_value = 0;
	calculate_hash(pass_code, 8, var_Hkey.index); // Calculates sha256 of the keys pressed in var_W*10 msec interval and stores it in var_Hkey
 	
	random_value = random_lcg();	
	
	calculate_hash(&random_value, 1, var_R.index);
	
	XOR_256(var_R.index, var_Hkey.index);
	
	calculate_hash(var_R.index, 8, var_T.index);
 	
	XOR_256(var_Salt.index, var_T.index);
		
	Start_W_timer();
}

void XOR_256(uint32_t *value1, uint32_t *value2)
{
	unsigned char i = 0;
	while (i++ < 8)
	{
		*value1 = *value1 ^ *value2;
		value1++;	value2++;
	}
}

void Start_W_timer(void)
{
	var_W = random_lcg() % 500 + 50;
	var_W_ticks = 0;	
}