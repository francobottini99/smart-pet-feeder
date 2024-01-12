#ifndef __WATERSENSOR_H__
#define __WATERSENSOR_H__

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

#include "ADCHandler.h"

#include "Utils.h"

#define WATERSENSOR_SAMPLES_PROM (uint8_t)10

void watersensor_init(ADC_CHANNEL_SELECTION adc_channel);
void watersensor_start_measuring(void);
uint32_t watersensor_read(void);

#endif /* __WATERSENSOR_H__ */
