#include "../inc/DMAHandler.h"

enum {FREE = 0, TAKEN = !FREE};

static struct
{
	void (*channel0)(DMA_Result);
	void (*channel1)(DMA_Result);
	void (*channel2)(DMA_Result);
	void (*channel3)(DMA_Result);
	void (*channel4)(DMA_Result);
	void (*channel5)(DMA_Result);
	void (*channel6)(DMA_Result);
	void (*channel7)(DMA_Result);
} dma_channel_handler = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static uint8_t dma_channel_status[DMA_TOTAL_CHANNELS] = {FREE};

void DMA_IRQHandler (void)
{
	for(uint8_t channel = 0; channel < 8; channel++)
	{
		if(GPDMA_IntGetStatus(GPDMA_STAT_INT, channel))
		{
			DMA_Result dma_result;

			if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, channel))
			{
				GPDMA_ClearIntPending (GPDMA_STATCLR_INTTC, channel);
				dma_result = DMA_OK;
			}
			else if(GPDMA_IntGetStatus(GPDMA_STAT_INTERR, channel))
			{
				GPDMA_ClearIntPending (GPDMA_STATCLR_INTERR, channel);
				dma_result = DMA_ERR;
			}

			GPDMA_ChannelCmd(channel, DISABLE);

			switch (channel)
			{
				case 0:
					LPC_GPDMACH0->DMACCConfig = 0;

					if(dma_channel_handler.channel0)
						(*dma_channel_handler.channel0)(dma_result);

					break;
				case 1:
					LPC_GPDMACH1->DMACCConfig = 0;

					if(dma_channel_handler.channel1)
						(*dma_channel_handler.channel1)(dma_result);

					break;
				case 2:
					LPC_GPDMACH2->DMACCConfig = 0;

					if(dma_channel_handler.channel2)
						(*dma_channel_handler.channel2)(dma_result);

					break;
				case 3:
					LPC_GPDMACH3->DMACCConfig = 0;

					if(dma_channel_handler.channel3)
						(*dma_channel_handler.channel3)(dma_result);

					break;
				case 4:
					LPC_GPDMACH4->DMACCConfig = 0;

					if(dma_channel_handler.channel4)
						(*dma_channel_handler.channel4)(dma_result);

					break;
				case 5:
					LPC_GPDMACH5->DMACCConfig = 0;

					if(dma_channel_handler.channel5)
						(*dma_channel_handler.channel5)(dma_result);

					break;
				case 6:
					LPC_GPDMACH6->DMACCConfig = 0;

					if(dma_channel_handler.channel6)
						(*dma_channel_handler.channel6)(dma_result);

					break;
				case 7:
					LPC_GPDMACH7->DMACCConfig = 0;

					if(dma_channel_handler.channel7)
						(*dma_channel_handler.channel7)(dma_result);

					break;
			}
		}
	}
}

int dma_add_handler(uint8_t channel, void (*handler)(DMA_Result))
{
	if(channel >= DMA_TOTAL_CHANNELS)
		return 1;

	if(dma_channel_status[channel] != FREE)
		return -1;

	switch (channel)
	{
		case 0:
			dma_channel_handler.channel0 = handler;
			break;
		case 1:
			dma_channel_handler.channel1 = handler;
			break;
		case 2:
			dma_channel_handler.channel2 = handler;
			break;
		case 3:
			dma_channel_handler.channel3 = handler;
			break;
		case 4:
			dma_channel_handler.channel4 = handler;
			break;
		case 5:
			dma_channel_handler.channel5 = handler;
			break;
		case 6:
			dma_channel_handler.channel6 = handler;
			break;
		case 7:
			dma_channel_handler.channel7 = handler;
			break;
	}

	dma_channel_status[channel] = TAKEN;

	NVIC_EnableIRQ(DMA_IRQn);

	return 0;
}

int dma_remove_handler(uint8_t channel)
{
	if(channel >= DMA_TOTAL_CHANNELS)
		return 1;

	switch (channel)
	{
		case 0:
			dma_channel_handler.channel0 = NULL;
			break;
		case 1:
			dma_channel_handler.channel1 = NULL;
			break;
		case 2:
			dma_channel_handler.channel2 = NULL;
			break;
		case 3:
			dma_channel_handler.channel3 = NULL;
			break;
		case 4:
			dma_channel_handler.channel4 = NULL;
			break;
		case 5:
			dma_channel_handler.channel5 = NULL;
			break;
		case 6:
			dma_channel_handler.channel6 = NULL;
			break;
		case 7:
			dma_channel_handler.channel7 = NULL;
			break;
	}

	dma_channel_status[channel] = FREE;

	if(is_dma_free())
		NVIC_DisableIRQ(DMA_IRQn);

	return 0;
}

int dma_get_free_channel(void)
{
	for(uint8_t channel = 0; channel < DMA_TOTAL_CHANNELS; channel++)
		if(dma_channel_status[channel] == FREE)
			return channel;

	return -1;
}

int is_dma_full(void)
{
	for(uint8_t channel = 0; channel < DMA_TOTAL_CHANNELS; channel++)
		if(dma_channel_status[channel] != TAKEN)
			return 0;

	return 1;
}

int is_dma_free(void)
{
	for(uint8_t channel = 0; channel < DMA_TOTAL_CHANNELS; channel++)
		if(dma_channel_status[channel] != FREE)
			return 0;

	return 1;
}
