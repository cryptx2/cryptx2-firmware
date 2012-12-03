/*
 * Salt.c
 *
 * Created: 25/Nov/12 6:30:07 PM
 *  Author: Waqas
 */ 

#include "Salt.h"
#include "sha256.h" 
#include "pbkdf2.h"
#include "aes_example.h"
#include "sha2.h"


//volatile uint32_t unlock_password[8] = {0};

volatile stored_values_t Stored_values = {{0}, {0}, {0}, {0}};
volatile uint32_t temp_password[8] = {0};
volatile uint32_t temp_password1[8] = {0};

//volatile uint32_t password_block [32] = {0};

volatile uint32_t *pass_code;
volatile bool device_unlocked = false;
volatile uint8_t passcode_byte_index = 0;
volatile unsigned long int rseed = 0;
volatile uint256_t var_Hkey;
volatile uint256_t var_R;
volatile uint256_t var_T;
volatile uint32_t var_W = 0xFFFFFFFF;
volatile uint32_t var_W_ticks = 0;
volatile uint256_t var_Salt;


uint32_t * encrypt_password(uint32_t *password)
{
	uint8_t temp_dk[32] = {0}, temp_dk_digest[32] = {0};
	pbkdf2_func((uint8_t *)password, temp_dk);
	sha256(temp_dk, 32, temp_dk_digest);
	apply_aes_encryption(&AVR32_AES, temp_dk_digest, 32, 0x00000000);
	return temp_dk_digest;	
}


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
	
	xor_func(var_R.index, var_Hkey.index, 8);
	
	calculate_hash(var_R.index, 8, var_T.index);
 	
	xor_func(var_Salt.index, var_T.index, 8);
	
	//if (enter_pressed)
	//{
		//save_salt_to_mcu();
	//}
	//else
	//{
		//Start_W_timer();	
	//}
	
}

void xor_func (uint32_t *value1, uint32_t *value2, uint8_t len)
{
	unsigned char i = 0;
	while (i++ < len)
	{
		*value1 = *value1 ^ *value2;
		value1++;	value2++;
	}
}

void save_salt_to_mcu(void)
{
	volatile salt_t *salt = &SALT_STRUCT;
	memcpy((char *)salt->salt_value_primary, (const char *)var_Salt.index, 32);
}

void Start_W_timer(void)
{
	var_W = random_lcg() % 500 + 50;
	var_W_ticks = 0;	
}