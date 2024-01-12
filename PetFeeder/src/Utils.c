#include "../inc/Utils.h"

uint8_t int_to_ascii_str(int value, char* buffer)
{
	uint8_t limit = strlen(buffer);
	uint8_t n = (uint8_t)(log10(abs(value)) + 1 + strlen(buffer));

	if(value < 0)
	{
		value = -value;
		buffer[limit++] = '-';
		n++;
	}

	if(value == 0)
	{
		buffer[limit++] = '0';
		buffer[limit] = '\0';
	}
	else
	{
		buffer[n] = '\0';

	    for (int i = n-1; i >= limit; --i)
	    {
	    	buffer[i] = (value % 10) + '0';
	    	value /= 10;
	    }
	}

	return strlen(buffer);
}

void fill_str(char* str, char c, uint16_t n)
{
	for (uint16_t i = strlen(str); i < n; i++)
		str[i] = c;
}

