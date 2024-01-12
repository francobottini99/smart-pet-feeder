#ifndef __UTILS_H__
#define __UTILS_H__

#include "LPC17xx.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#define LOW 			(uint8_t)0
#define HIHG 			(uint8_t)1

#define PINMODE_OUTPUT 	(uint8_t)1
#define PINMODE_INPUT 	(uint8_t)0

#define RISING_EDGE 	(uint8_t)0
#define RISING_FALLING 	(uint8_t)1

uint8_t int_to_ascii_str(int value, char* buffer);
void fill_str(char* str, char c, uint16_t n);

#endif /* __UTILS_H__ */
