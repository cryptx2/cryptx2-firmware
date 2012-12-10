/*
 * aes_example.h
 *
 * Created: 06/Oct/12 11:59:54 PM
 *  Author: Waqas
 */ 


#ifndef AES_EXAMPLE_H_
#define AES_EXAMPLE_H_

#include "aes.h"

void aes_task (void);
void aes_init (unsigned char Processing_Mode);
void apply_aes_encryption (volatile avr32_aes_t *aes, uint32_t *aes_in_data, uint32_t *aes_key, uint16_t len, uint32_t sector_id);
void apply_aes_decryption (volatile avr32_aes_t *aes, uint32_t *aes_in_data, uint32_t *aes_key, uint16_t len, uint32_t sector_id);

#endif /* AES_EXAMPLE_H_ */