/*
 * sha256.h
 *
 * Created: 24/Nov/12 8:41:39 PM
 *  Author: Waqas
 */ 


#ifndef SHA256_H_
#define SHA256_H_

#include "compiler.h"


extern const unsigned long int CipherKey256[8];


void calculate_hash (uint32_t *input_array, uint8_t length, uint32_t *output_array);


#endif /* SHA256_H_ */