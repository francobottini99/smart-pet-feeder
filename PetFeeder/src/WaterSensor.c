#include "../inc/WaterSensor.h"

void watersensor_config_pin(void);
void watersensor_config_adc(void);
void watersensor_adc_decode_channel(void);

struct
{
	PINSEL_CFG_Type pinsel_config;

	uint8_t port;
	uint8_t dt_pin;
	uint8_t pin_func;
} watersensor_pin;

struct
{
	ADC_CHANNEL_SELECTION adc_channel;
	uint8_t reserved[3];

	uint32_t conversion_rate;
} watersensor_adc;

uint16_t watersensor_measurings[WATERSENSOR_SAMPLES_PROM];
uint32_t watersensor_last_prom;

void watersensor_handler(void)
{
	static uint8_t i = 0;

	watersensor_measurings[i] = ADC_ChannelGetData(LPC_ADC, (uint8_t)watersensor_adc.adc_channel);

	if(i < WATERSENSOR_SAMPLES_PROM)
		i++;
	else
		i = 0;
}

void watersensor_config_pin()
{
	watersensor_pin.pinsel_config.Portnum   = watersensor_pin.port;
	watersensor_pin.pinsel_config.Pinnum    = watersensor_pin.dt_pin;
	watersensor_pin.pinsel_config.Funcnum   = watersensor_pin.pin_func;
	watersensor_pin.pinsel_config.Pinmode   = PINSEL_PINMODE_TRISTATE;
	watersensor_pin.pinsel_config.OpenDrain = PINSEL_PINMODE_NORMAL;

    PINSEL_ConfigPin(&watersensor_pin.pinsel_config);
}

void watersensor_config_adc()
{
	ADC_Init(LPC_ADC, watersensor_adc.conversion_rate);
	ADC_IntConfig(LPC_ADC, watersensor_adc.adc_channel, ENABLE);
	ADC_ChannelCmd(LPC_ADC, watersensor_adc.adc_channel, ENABLE);

	adc_add_handler((uint8_t)watersensor_adc.adc_channel, watersensor_handler);
}

void watersensor_init(ADC_CHANNEL_SELECTION adc_channel)
{
	watersensor_adc.conversion_rate = 200000;
	watersensor_adc.adc_channel = adc_channel;

	watersensor_adc_decode_channel();
	watersensor_config_pin();
	watersensor_config_adc();
}

void watersensor_adc_decode_channel(void)
{
	switch(watersensor_adc.adc_channel)
	{
		case ADC_CHANNEL_0:
			watersensor_pin.port = 0;
			watersensor_pin.pin_func = 1;
			watersensor_pin.dt_pin = 23;

			break;
		case ADC_CHANNEL_1:
			watersensor_pin.port = 0;
			watersensor_pin.pin_func = 1;
			watersensor_pin.dt_pin = 24;

			break;
		case ADC_CHANNEL_2:
			watersensor_pin.port = 0;
			watersensor_pin.pin_func = 1;
			watersensor_pin.dt_pin = 25;

			break;
		case ADC_CHANNEL_3:
			watersensor_pin.port = 0;
			watersensor_pin.pin_func = 1;
			watersensor_pin.dt_pin = 26;

			break;
		case ADC_CHANNEL_4:
			watersensor_pin.port = 1;
			watersensor_pin.pin_func = 3;
			watersensor_pin.dt_pin = 30;

			break;
		case ADC_CHANNEL_5:
			watersensor_pin.port = 1;
			watersensor_pin.pin_func = 3;
			watersensor_pin.dt_pin = 31;

			break;
		case ADC_CHANNEL_6:
			watersensor_pin.port = 0;
			watersensor_pin.pin_func = 2;
			watersensor_pin.dt_pin = 2;

			break;
		case ADC_CHANNEL_7:
			watersensor_pin.port = 0;
			watersensor_pin.pin_func = 2;
			watersensor_pin.dt_pin = 3;

			break;
	}
}

void watersensor_start_measuring(void)
{
	ADC_StartCmd(LPC_ADC, ADC_START_NOW);
	NVIC_EnableIRQ(ADC_IRQn);
}

uint32_t watersensor_read(void)
{
	uint32_t sum = 0;

	for(uint16_t i = 0; i < WATERSENSOR_SAMPLES_PROM; i++)
		sum += watersensor_measurings[i];

	watersensor_last_prom = sum / WATERSENSOR_SAMPLES_PROM;

	return (watersensor_last_prom * 100) / 4095;
}
