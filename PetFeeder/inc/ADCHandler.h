#ifndef __ADCHANDLER_H__
#define __ADCHANDLER_H__

#include "lpc17xx_adc.h"
#include "Utils.h"

#define ADC_TOTAL_CHANNELS (uint8_t)8

int adc_add_handler(uint8_t channel, void (*handler)(void));
int adc_remove_handler(uint8_t channel);

#endif /* __ADCHANDLER_H__ */
