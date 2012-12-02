/*
 * pbkdf2.h
 *
 * Created: 11/27/2012 9:22:05 PM
 *  Author: mwaqas
 */ 


#ifndef PBKDF2_H_
#define PBKDF2_H_

#include <string.h>
#include "compiler.h"


void pbkdf2_func(uint8_t *password, uint8_t *derived_key);

#endif /* PBKDF2_H_ */