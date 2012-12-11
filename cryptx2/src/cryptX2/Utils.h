/*
 * Utils.h
 *
 * Created: 11/28/2012 7:59:58 PM
 *  Author: mwaqas
 */ 


#ifndef UTILS_H_
#define UTILS_H_

#include "compiler.h"

typedef struct
{
	unsigned int stored_value_crc_status : 1;
} system_status_t; 

extern volatile system_status_t stSystemStatus;


void Init_System_Status(void);


#endif /* UTILS_H_ */