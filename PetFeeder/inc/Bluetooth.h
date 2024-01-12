#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#define BLUETOOTH_PACKAGE_SIZE (uint8_t)27

#include <stdlib.h>
#include <string.h>

#include "lpc17xx_uart.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_pinsel.h"

#include "DMAHandler.h"
#include "Utils.h"

typedef enum
{
	BT_PACKAGE_WEIGHT,
	BT_PACKAGE_WATERLEVEL,
	BT_PACKAGE_PLAIN_TEXT,
	BT_PACKAGE_TARA,
	BT_PACKAGE_SCALE,
	BT_PACKAGE_RELOAD,
	BT_PACKAGE_UNITS,
	BT_PACKAGE_GATE_STATUS,
	BT_PACKAGE_KEYBOARD_INPUT,
	BT_PACKAGE_KEYBOARD_MODE,
	BT_PACKAGE_KEYBOARD_DELETE,
	BT_PACKAGE_KEYBOARD_CANCEL,
	BT_PACKAGE_ERROR_BAD_REFERENCE,
	BT_PACKAGE_ERROR_BAD_RELOAD,
	BT_PACKAGE_STATUS_CONFIG
} bluetooth_package_type;

void bluetooth_init(LPC_UART_TypeDef *uart, uint8_t rx_dma_channel);
int bluetooh_send_package(bluetooth_package_type type, void *data);
int bluetooth_read_package(bluetooth_package_type* type, char* buffer);

#endif /* __BLUETOOTH_H__ */
