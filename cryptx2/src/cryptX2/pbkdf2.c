/*
 * pbkdf2.c
 *
 * Created: 11/27/2012 9:21:53 PM
 *  Author: mwaqas
 */ 

#include "pbkdf2.h"
#include "Salt.h"
#include "Utils.h"

#define hLen	32					// hLen denotes the length in octets of the pseudorandom function output i.e. SHA256 (256/8)
#define dkLen	64					// hLen * 2 (can be upto ((2^32 - 1) * hLen)
#define TOTAL_ITERATIONS	500
//#define PBKDF2	void pbkdf2_func
//#define P		unsigned char *password
//#define S		unsigned char *Salt
//#define c		unsigned short int iterations

volatile uint256_t var_Password;


void pbkdf2_func(void)
{
	unsigned short int iteration_count;
	unsigned char mac[hLen];
	unsigned char Ubuffer[hLen+4] = {0};
	unsigned char Tbuffer[dkLen] = {0};
	unsigned char Fbuffer[hLen] = {0};
	unsigned char total_blocks = dkLen / hLen, block_num = 1;
	unsigned char *Salt = (unsigned char *)var_Salt.index;
	
	while (block_num <= total_blocks)
	{
		iteration_count = 0;
		while (iteration_count < TOTAL_ITERATIONS)			// TOTAL_ITERATIONS represents 'c' in PRF
		{
			unsigned short int i;
			if (iteration_count == 0)
			{
				unsigned short int i;
				for (i = 0; i < 32; i++)
				{
					Ubuffer[i] = Salt[i];
				}
				Ubuffer[i++] = (block_num >> 24) & 0xFF;
				Ubuffer[i++] = (block_num >> 16) & 0xFF;
				Ubuffer[i++] = (block_num >> 8) & 0xFF;
				Ubuffer[i] = (block_num >> 0) & 0xFF;
				hmac_sha256((const unsigned char *)var_Password.index, 32, Ubuffer, 36, mac, 32);
			}
			else
			{
				hmac_sha256((const unsigned char *)var_Password.index, 32, Ubuffer, 32, mac, 32);
					
			}
			
			xor_func(Fbuffer, mac, 8);
			
			for (i = 0; i < hLen; i++)
			{
				Ubuffer[i] = mac[i];
			}
			
			iteration_count++;
		}
		xor_func(&Tbuffer[block_num * 2], Fbuffer, 32);
		memset(Fbuffer, 0, 32);		
		block_num++;
	}
}	