/*
 * Utils.c
 *
 * Created: 11/28/2012 7:48:59 PM
 *  Author: mwaqas
 */ 


void memset(unsigned char *dst, unsigned int value, unsigned int len)
{
	unsigned int i = 0;
	while (i < len)
	{
		dst[i++] = value;
	}
}