/*
 * Salt.h
 *
 * Created: 25/Nov/12 6:30:32 PM
 *  Author: Waqas
 */ 


#ifndef SALT_H_
#define SALT_H_


#include "compiler.h"


#define SALT_PAGE_NUMBER		100			// it can be from 1-512 for 256 KBytes of Flash
#define SALT_MEM_ADDRESS		0x80000000 + (512L * SALT_PAGE_NUMBER)
#define SALT_STRUCT				(*((volatile salt_t *)SALT_MEM_ADDRESS))

#define PASSWORD_SIZE						8
#define UNLOCK_PASSWORD_INDEX				0
#define HIDDEN_DATA_UNLOCK_PASSWORD_INDEX	UNLOCK_PASSWORD_INDEX + 8
#define PANIC_MODE_PASSWORD_INDEX			HIDDEN_DATA_UNLOCK_PASSWORD_INDEX + 8
#define DEVICE_ID_CONFIRM_INDEX				PANIC_MODE_PASSWORD_INDEX + 8


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

typedef struct
{
	uint32_t unlock_password[8];
	//uint8_t  unlock_password_crc;
	uint32_t hidden_data_unlock_password [8];
	//uint8_t  hidden_data_unlock_password_crc;
	uint32_t panic_mode [8];
	//uint8_t  panic_mode_crc;
	uint32_t device_id_confirm [8];
	//uint8_t  device_id_confirm_crc;
	uint8_t device_id_sequence[32];
	//uint8_t  device_id_sequence_crc;
	uint32_t salt[8];
	//uint8_t  salt_crc;
	uint16_t  block_crc;	
} stored_values_t;

extern volatile uint32_t password_block [32];

extern volatile uint32_t *pass_code;
extern volatile bool device_unlocked;
extern volatile uint8_t passcode_byte_index;

extern volatile uint32_t var_W;
extern volatile uint32_t var_W_ticks;
extern volatile uint256_t var_Hkey;
extern volatile uint256_t var_R;
extern volatile uint256_t var_T;
extern volatile uint256_t var_Salt;

extern volatile stored_values_t Stored_values_ram;
extern volatile uint32_t temp_password[8];
extern volatile uint32_t temp_password1[8];


uint32_t random_lcg (void);
void calculate_salt(void);
void xor_func (uint32_t *value1, uint32_t *value2, uint8_t len);
void Start_W_timer(void);
void encrypt_password(uint32_t *password, uint32_t *digest);
void save_salt_to_mcu(void);
void Update_stored_values(void);
void Load_stored_values(void);
void Calculate_block_crc(void);
void save_sequence_to_mcu(void);
void hash_the_password(uint32_t *password, uint32_t *digest);

#endif /* SALT_H_ */