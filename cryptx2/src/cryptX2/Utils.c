/*
 * Utils.c
 *
 * Created: 11/28/2012 7:48:59 PM
 *  Author: mwaqas
 */ 

#include "Utils.h"


volatile system_status_t stSystemStatus;


void Init_System_Status(void)
{
	stSystemStatus.stored_value_crc_status = 0;
	stSystemStatus.unlock_password_status = 0;
	stSystemStatus.read_only_mode_0_status = 0;
	stSystemStatus.read_only_mode_1_status = 0;
}
//void memset(unsigned char *dst, unsigned int value, unsigned int len)
//{
	//unsigned int i = 0;
	//while (i < len)
	//{
		//dst[i++] = value;
	//}
//}