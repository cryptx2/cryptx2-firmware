/*
 * sha256.c
 *
 * Created: 24/Nov/12 8:41:14 PM
 *  Author: Waqas
 */ 
#include "sha256.h"
#include "compiler.h"


#define XOR ^
#define ARRAY true
#define STRING false
#define REVERSE_DWORD(x) ((x >> 24) & 0x000000FF) | ((x >> 8) & 0x0000FF00) | ((x << 8) & 0x00FF0000) | ((x << 24) & 0xFF000000)




// CipherKey256 array
// the_key = 256'h603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4;
const unsigned long int CipherKey256[8] = {
	0x603deb10,
	0x15ca71be,
	0x2b73aef0,
	0x857d7781,
	0x1f352c07,
	0x3b6108d7,
	0x2d9810a3,
	0x0914dff4
};


const unsigned long int Initial_Hash [] =  {
	0x6a09e667,
	0xbb67ae85,
	0x3c6ef372,
	0xa54ff53a,
	0x510e527f,
	0x9b05688c,
	0x1f83d9ab,
	0x5be0cd19
};

const unsigned long int Hash_Constant [] = {
	0x428a2f98,	0x71374491,	0xb5c0fbcf,	0xe9b5dba5,
	0x3956c25b,	0x59f111f1,	0x923f82a4,	0xab1c5ed5,
	0xd807aa98,	0x12835b01,	0x243185be,	0x550c7dc3,
	0x72be5d74,	0x80deb1fe,	0x9bdc06a7,	0xc19bf174,
	0xe49b69c1,	0xefbe4786,	0x0fc19dc6,	0x240ca1cc,
	0x2de92c6f,	0x4a7484aa,	0x5cb0a9dc,	0x76f988da,
	0x983e5152,	0xa831c66d,	0xb00327c8,	0xbf597fc7,
	0xc6e00bf3,	0xd5a79147,	0x06ca6351,	0x14292967,
	0x27b70a85,	0x2e1b2138,	0x4d2c6dfc,	0x53380d13,
	0x650a7354,	0x766a0abb,	0x81c2c92e,	0x92722c85,
	0xa2bfe8a1,	0xa81a664b,	0xc24b8b70,	0xc76c51a3,
	0xd192e819,	0xd6990624,	0xf40e3585,	0x106aa070,
	0x19a4c116,	0x1e376c08,	0x2748774c,	0x34b0bcb5,
	0x391c0cb3,	0x4ed8aa4a,	0x5b9cca4f,	0x682e6ff3,
	0x748f82ee,	0x78a5636f,	0x84c87814,	0x8cc70208,
	0x90befffa,	0xa4506ceb,	0xbef9a3f7,	0xc67178f2
};

typedef struct
{
	unsigned long int a;
	unsigned long int b;
	unsigned long int c;
	unsigned long int d;
	unsigned long int e;
	unsigned long int f;
	unsigned long int g;
	unsigned long int h;
} CHUNK_t;

unsigned char Padded_Message[64] = {0};
unsigned long Message_Schedule_W[64] = {0};
CHUNK_t chunk;

//void calculate_hash (void);
void PadtheMessage(uint8_t message_length);
unsigned long Rotate_right (unsigned long number, int bits);



 void calculate_hash (uint32_t *input_array, uint8_t length, uint32_t *output_array)
 {
	 int i;
	 unsigned long int S0, S1;
	 //bool Input_type = ARRAY;
	 unsigned long int *Key_ptr = (uint32_t *)&Padded_Message[0];
	 unsigned long int *W, *M;
	 unsigned long int *chunk_ptr = (unsigned long int *)&chunk;
	 //strcpy((char *)Padded_Message, (const char *)CipherKey256);
	 for (i = 0; i < length; i++)
	 {
		 Key_ptr[i] = input_array[i];
	 }
	 
	 PadtheMessage(length << 2);
	 
	 
	 W = Message_Schedule_W;
	 M = (unsigned long int *)Padded_Message;
	 for (i = 0; i < 16; i++)
	 {
		W[i] = M[i];
	 }
	 //strncpy((char *)Message_Schedule_W, (const char *)Padded_Message, sizeof(Padded_Message));

	 for (i = 16; i < 64; i++)
	 {
		 S0 = Rotate_right(Message_Schedule_W[i - 15], 7) XOR Rotate_right(Message_Schedule_W[i - 15], 18) XOR (Message_Schedule_W[i - 15] >> 3);
		 S1 = Rotate_right(Message_Schedule_W[i - 2], 17) XOR Rotate_right(Message_Schedule_W[i - 2], 19) XOR (Message_Schedule_W[i - 2] >> 10);
		 Message_Schedule_W[i] = Message_Schedule_W[i - 16] + S0 + Message_Schedule_W[i - 7] + S1;
	 }
	 
	 
	 for (i = 0; i < 8; i++)
	 {
		 chunk_ptr[i] = Initial_Hash[i];
	 }
	 
	 for (i = 0; i < 64; i++)
	 {
		 unsigned long int maj, t2, ch, t1;
		 S0 = Rotate_right(chunk.a, 2) XOR Rotate_right(chunk.a, 13) XOR Rotate_right(chunk.a, 22);
		 maj = (chunk.a & chunk.b) XOR (chunk.a & chunk.c) XOR (chunk.b & chunk.c);
		 t2 = S0 + maj;
		 S1 = Rotate_right(chunk.e, 6) XOR Rotate_right(chunk.e, 11) XOR Rotate_right(chunk.e, 25);
		 ch = (chunk.e & chunk.f) XOR ((~chunk.e) & chunk.g);
		 t1 = chunk.h + S1 + ch + Hash_Constant[i] + Message_Schedule_W[i];
		 
		 chunk.h = chunk.g;
		 chunk.g = chunk.f;
		 chunk.f = chunk.e;
		 chunk.e = chunk.d + t1;
		 chunk.d = chunk.c;
		 chunk.c = chunk.b;
		 chunk.b = chunk.a;
		 chunk.a = t1 + t2;
	 }

	 for (i = 0; i < 8; i++)
	 {
		 output_array[i] = Initial_Hash[i] + chunk_ptr[i];
	 }
 }


 void PadtheMessage(uint8_t message_length)
 {
	 unsigned char i;
	 unsigned short int message_length_bits;
	 unsigned long int *ptrPadded_Message = (unsigned long int *)&Padded_Message[0];

	 message_length_bits = message_length << 3;
	 Padded_Message[message_length++] = 0x80;

	 for (i = message_length; i < 64; i++)
	 {
		Padded_Message[i] = 0x00;
	 }

	 //ptrPadded_Message = (unsigned long int *)&Padded_Message[60];
	 ptrPadded_Message[15] = (unsigned long int)message_length_bits;
	 //Padded_Message[62] = *((unsigned char *)&message_length_bits);
	 //Padded_Message[63] = *((unsigned char *)&message_length_bits + 1);
 }

 unsigned long int Rotate_right (unsigned long int number, int bits)
 {
	 unsigned long int bit_mask = ((unsigned long int)2 << bits) - 1;
	 return ((number >> bits) | ((number & bit_mask) << (32 - bits)));
 }
