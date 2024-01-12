#include "../inc/ADCHandler.h"

static struct
{
	void (*channel0)(void);
	void (*channel1)(void);
	void (*channel2)(void);
	void (*channel3)(void);
	void (*channel4)(void);
	void (*channel5)(void);
	void (*channel6)(void);
	void (*channel7)(void);
} adc_channel_handler = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

void ADC_IRQHandler (void)
{
	for (uint8_t channel = 0; channel < 8; channel++)
	{
		if(ADC_ChannelGetStatus(LPC_ADC, channel, ADC_DATA_DONE))
		{
			switch (channel)
			{
				case 0:
					if(adc_channel_handler.channel0)
						(*adc_channel_handler.channel0)();

					break;
				case 1:
					if(adc_channel_handler.channel1)
						(*adc_channel_handler.channel1)();

					break;
				case 2:
					if(adc_channel_handler.channel2)
						(*adc_channel_handler.channel2)();

					break;
				case 3:
					if(adc_channel_handler.channel3)
						(*adc_channel_handler.channel3)();

					break;
				case 4:
					if(adc_channel_handler.channel4)
						(*adc_channel_handler.channel4)();

					break;
				case 5:
					if(adc_channel_handler.channel5)
						(*adc_channel_handler.channel5)();

					break;
				case 6:
					if(adc_channel_handler.channel6)
						(*adc_channel_handler.channel6)();

					break;
				case 7:
					if(adc_channel_handler.channel7)
						(*adc_channel_handler.channel7)();

					break;
			}
		}

		NVIC_DisableIRQ(ADC_IRQn);
	}
}

int adc_add_handler(uint8_t channel, void (*handler)(void))
{
	if(channel >= ADC_TOTAL_CHANNELS)
		return 1;

	switch (channel)
	{
		case 0:
			adc_channel_handler.channel0 = handler;
			break;
		case 1:
			adc_channel_handler.channel1 = handler;
			break;
		case 2:
			adc_channel_handler.channel2 = handler;
			break;
		case 3:
			adc_channel_handler.channel3 = handler;
			break;
		case 4:
			adc_channel_handler.channel4 = handler;
			break;
		case 5:
			adc_channel_handler.channel5 = handler;
			break;
		case 6:
			adc_channel_handler.channel6 = handler;
			break;
		case 7:
			adc_channel_handler.channel7 = handler;
			break;
	}

	return 0;
}

int adc_remove_handler(uint8_t channel)
{
	if(channel >= ADC_TOTAL_CHANNELS)
		return 1;

	switch (channel)
	{
		case 0:
			adc_channel_handler.channel0 = NULL;
			break;
		case 1:
			adc_channel_handler.channel1 = NULL;
			break;
		case 2:
			adc_channel_handler.channel2 = NULL;
			break;
		case 3:
			adc_channel_handler.channel3 = NULL;
			break;
		case 4:
			adc_channel_handler.channel4 = NULL;
			break;
		case 5:
			adc_channel_handler.channel5 = NULL;
			break;
		case 6:
			adc_channel_handler.channel6 = NULL;
			break;
		case 7:
			adc_channel_handler.channel7 = NULL;
			break;
	}

	return 0;
}
