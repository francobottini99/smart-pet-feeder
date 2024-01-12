#ifndef __DMAHANDLER_H__
#define __DMAHANDLER_H__

#include "lpc17xx_gpdma.h"
#include "Utils.h"

#define DMA_TOTAL_CHANNELS (uint8_t)8

typedef enum {DMA_OK = 0, DMA_ERR = !DMA_OK} DMA_Result;

int dma_get_free_channel(void);
int dma_add_handler(uint8_t channel, void (*handler)(DMA_Result));
int dma_remove_handler(uint8_t channel);
int is_dma_full(void);
int is_dma_free(void);

#endif /* __DMAHANDLER_H__ */
