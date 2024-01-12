#include "../inc/Bluetooth.h"

#define TX_BUFFER_SIZE (uint8_t)32
#define RX_BUFFER_SIZE (uint8_t)32

struct
{
	LPC_UART_TypeDef *LPC_uart;

	UART_CFG_Type config;
	UART_FIFO_CFG_Type fifo_config;

	PINSEL_CFG_Type rx_pinsel_config;
	PINSEL_CFG_Type tx_pinsel_config;

	uint8_t rx_pin;
	uint8_t tx_pin;

	uint8_t port_num;
	uint8_t reserved[1];
} bluetooth_uart;

struct
{
	GPDMA_Channel_CFG_Type tx_channel_config;
	GPDMA_Channel_CFG_Type rx_channel_config;

	GPDMA_LLI_Type tx_lli;
	GPDMA_LLI_Type rx_lli;

	int8_t tx_channel;
	int8_t rx_channel;

	uint8_t in_rx_flag;
	uint8_t in_tx_flag;

	char tx_buffer[TX_BUFFER_SIZE];
	char rx_buffer[RX_BUFFER_SIZE];
} bluetooth_dma;

typedef struct bluetooth_package
{
	struct bluetooth_package* next;
	bluetooth_package_type type;
	char data[BLUETOOTH_PACKAGE_SIZE];
} bluetooth_package;

bluetooth_package** tx_queue = NULL;
bluetooth_package** rx_queue = NULL;

void bluetooth_rx_handler(DMA_Result dma_result);
void bluetooth_tx_handler(DMA_Result dma_result);
void bluetooh_dma_start_tx();

bluetooth_package* bluetooh_get_last_package(bluetooth_package** queue);
void bluetooh_put_package(bluetooth_package** queue, bluetooth_package* package);
void bluetooh_pop_package(bluetooth_package** queue, bluetooth_package* buffer);

void bluetooth_pin_config(void);
void bluetooth_uart_config(void);
void bluetooth_tx_dma_config(void);
void bluetooth_dma_config(void);

/** ------------ HANDLER TX AND RX BLUETOOTH ------------ **/

void bluetooth_rx_handler(DMA_Result dma_result)
{
	bluetooth_package* package = malloc(sizeof(bluetooth_package));

	char *token = strtok(bluetooth_dma.rx_buffer, ";");

	package->type = (bluetooth_package_type)atoi(token);

	token = strtok(NULL, ";");

	strcpy(package->data, token);

	for(uint8_t i = 0; i < BLUETOOTH_PACKAGE_SIZE; i++)
		if(package->data[i] == '#')
			package->data[i] = '\0';

	bluetooh_put_package(rx_queue, package);

	bluetooth_rx_dma_config();

	GPDMA_ChannelCmd(bluetooth_dma.rx_channel, ENABLE);
}

void bluetooth_tx_handler(DMA_Result dma_result)
{
	bluetooth_dma.in_tx_flag = 0;

	if(*tx_queue)
		bluetooh_dma_start_tx();
	else
	{
		dma_remove_handler(bluetooth_dma.tx_channel);
		bluetooth_dma.tx_channel = -1;
	}
}

void bluetooh_dma_start_tx()
{
	if(!bluetooth_dma.in_tx_flag)
	{
		bluetooth_package buffer;

		bluetooth_dma.in_tx_flag = 1;

		if(bluetooth_dma.tx_channel < 0)
		{
			while(is_dma_full());

			bluetooth_dma.tx_channel = dma_get_free_channel();

			dma_add_handler(bluetooth_dma.tx_channel, bluetooth_tx_handler);
		}

		for (uint8_t i = 0; i < TX_BUFFER_SIZE; i++)
			bluetooth_dma.tx_buffer[i] = '\0';

		bluetooh_pop_package(tx_queue, &buffer);

		bluetooth_dma.tx_buffer[0] = '|';
		bluetooth_dma.tx_buffer[1] = '\0';

		uint8_t i = int_to_ascii_str((int)buffer.type, bluetooth_dma.tx_buffer);

		bluetooth_dma.tx_buffer[i++] = ';';
		bluetooth_dma.tx_buffer[i] = '\0';

		strcat(bluetooth_dma.tx_buffer, buffer.data);

		bluetooth_dma.tx_buffer[strlen(bluetooth_dma.tx_buffer)] = '|';
		bluetooth_dma.tx_buffer[strlen(bluetooth_dma.tx_buffer) + 1] = '\0';

		bluetooth_tx_dma_config();

		GPDMA_ChannelCmd(bluetooth_dma.tx_channel, ENABLE);
	}
}

/** ------------ HANDLER TX AND RX BLUETOOTH ------------ **/

/** ------------ PACKAGES QUEUE MANAGMENT  ------------ **/

bluetooth_package* bluetooh_get_last_package(bluetooth_package** queue)
{
	if(!*queue)
		return NULL;

	bluetooth_package* last = *queue;

	while(last->next)
		last = last->next;

	return last;
}

void bluetooh_pop_package(bluetooth_package** queue, bluetooth_package* buffer)
{
	bluetooth_package* aux = *queue;

	*buffer = **queue;

	*queue = (*queue)->next;

	free(aux);
}

void bluetooh_put_package(bluetooth_package** queue, bluetooth_package* package)
{
	bluetooth_package* last = bluetooh_get_last_package(queue);

	package->next = NULL;

	if(!last)
		*queue = package;
	else
		last->next = package;
}

/** ------------ PACKAGES QUEUE MANAGMENT  ------------ **/

/** ------------ CONFIG BLUETOOTH  ------------ **/

void bluetooth_pin_config(void)
{
	bluetooth_uart.rx_pinsel_config.OpenDrain 	= PINSEL_PINMODE_NORMAL;
	bluetooth_uart.tx_pinsel_config.OpenDrain	= PINSEL_PINMODE_NORMAL;
	bluetooth_uart.rx_pinsel_config.Pinmode 	= PINSEL_PINMODE_TRISTATE;
	bluetooth_uart.tx_pinsel_config.Pinmode 	= PINSEL_PINMODE_TRISTATE;
	bluetooth_uart.rx_pinsel_config.Pinnum	 	= bluetooth_uart.rx_pin;
	bluetooth_uart.tx_pinsel_config.Pinnum 		= bluetooth_uart.tx_pin;

	if(bluetooth_uart.LPC_uart == LPC_UART3)
	{
		bluetooth_uart.rx_pinsel_config.Funcnum = 2;
		bluetooth_uart.tx_pinsel_config.Funcnum = 2;
	}
	else
	{
		bluetooth_uart.rx_pinsel_config.Funcnum = 1;
		bluetooth_uart.tx_pinsel_config.Funcnum = 1;
	}

	PINSEL_ConfigPin(&bluetooth_uart.rx_pinsel_config);
	PINSEL_ConfigPin(&bluetooth_uart.tx_pinsel_config);
}

void bluetooth_uart_config(void)
{
	bluetooth_uart.config.Baud_rate = 9600;
	bluetooth_uart.config.Databits 	= UART_DATABIT_8;
	bluetooth_uart.config.Parity 	= UART_PARITY_NONE;
	bluetooth_uart.config.Stopbits 	= UART_STOPBIT_1;

	bluetooth_uart.fifo_config.FIFO_DMAMode 	= ENABLE;
	bluetooth_uart.fifo_config.FIFO_Level 		= UART_FIFO_TRGLEV0;
	bluetooth_uart.fifo_config.FIFO_ResetRxBuf 	= ENABLE;
	bluetooth_uart.fifo_config.FIFO_ResetTxBuf	= ENABLE;

	UART_Init(bluetooth_uart.LPC_uart, &bluetooth_uart.config);
	UART_FIFOConfig(bluetooth_uart.LPC_uart, &bluetooth_uart.fifo_config);
	UART_TxCmd(bluetooth_uart.LPC_uart, ENABLE);
}

void bluetooth_tx_dma_config(void)
{
	bluetooth_dma.tx_lli.SrcAddr = (uint32_t)&(bluetooth_uart.LPC_uart->THR);
	bluetooth_dma.tx_lli.DstAddr = (uint32_t)&bluetooth_dma.tx_buffer;
	bluetooth_dma.tx_lli.NextLLI = (uint32_t)&bluetooth_dma.tx_lli;
	bluetooth_dma.tx_lli.Control = TX_BUFFER_SIZE | (1<<27);

	bluetooth_dma.tx_channel_config.ChannelNum 		= bluetooth_dma.tx_channel;
	bluetooth_dma.tx_channel_config.SrcMemAddr 		= (uint32_t)&bluetooth_dma.tx_buffer;
	bluetooth_dma.tx_channel_config.DstMemAddr 		= 0;
	bluetooth_dma.tx_channel_config.TransferSize 	= TX_BUFFER_SIZE;
	bluetooth_dma.tx_channel_config.TransferWidth 	= 0;
	bluetooth_dma.tx_channel_config.TransferType 	= GPDMA_TRANSFERTYPE_M2P;
	bluetooth_dma.tx_channel_config.SrcConn 		= 0;
	bluetooth_dma.tx_channel_config.DstConn 		= GPDMA_CONN_UART0_Tx + 2 * bluetooth_uart.port_num;
	bluetooth_dma.tx_channel_config.DMALLI 			= (uint32_t)&bluetooth_dma.tx_lli;

	GPDMA_Setup(&bluetooth_dma.tx_channel_config);
}

void bluetooth_rx_dma_config(void)
{
	bluetooth_dma.rx_lli.SrcAddr = (uint32_t)&(bluetooth_uart.LPC_uart->RBR);
	bluetooth_dma.rx_lli.DstAddr = (uint32_t)&bluetooth_dma.rx_buffer;
	bluetooth_dma.rx_lli.NextLLI = (uint32_t)&bluetooth_dma.rx_lli;
	bluetooth_dma.rx_lli.Control = RX_BUFFER_SIZE | (1<<27);

	bluetooth_dma.rx_channel_config.ChannelNum 		= bluetooth_dma.rx_channel;
	bluetooth_dma.rx_channel_config.SrcMemAddr 		= 0;
	bluetooth_dma.rx_channel_config.DstMemAddr 		= (uint32_t)&bluetooth_dma.rx_buffer;
	bluetooth_dma.rx_channel_config.TransferSize 	= RX_BUFFER_SIZE;
	bluetooth_dma.rx_channel_config.TransferWidth 	= 0;
	bluetooth_dma.rx_channel_config.TransferType 	= GPDMA_TRANSFERTYPE_P2M;
	bluetooth_dma.rx_channel_config.SrcConn 		= GPDMA_CONN_UART0_Rx + 2 * bluetooth_uart.port_num;
	bluetooth_dma.rx_channel_config.DstConn 		= 0;
	bluetooth_dma.rx_channel_config.DMALLI 			= (uint32_t)&bluetooth_dma.rx_lli;

	GPDMA_Setup(&bluetooth_dma.rx_channel_config);
}

/** ------------ CONFIG BLUETOOTH  ------------ **/

/** ------------ PUBLIC FUNCTIONS ------------ **/

void bluetooth_init(LPC_UART_TypeDef *uart, uint8_t rx_dma_channel)
{
	tx_queue = malloc(sizeof(bluetooth_package*));
	rx_queue = malloc(sizeof(bluetooth_package*));

	*tx_queue = NULL;
	*rx_queue = NULL;

	bluetooth_uart.LPC_uart = uart;

	bluetooth_dma.tx_channel = -1;
	bluetooth_dma.rx_channel = rx_dma_channel;

	if(bluetooth_uart.LPC_uart == (LPC_UART_TypeDef*)LPC_UART0)
	{
		bluetooth_uart.rx_pin = 3;
		bluetooth_uart.tx_pin = 2;
		bluetooth_uart.port_num = 0;
	}
	else if(bluetooth_uart.LPC_uart == (LPC_UART_TypeDef*)LPC_UART1)
	{
		bluetooth_uart.rx_pin = 16;
		bluetooth_uart.tx_pin = 15;
		bluetooth_uart.port_num = 1;
	}
	else if(bluetooth_uart.LPC_uart == LPC_UART2)
	{
		bluetooth_uart.rx_pin = 11;
		bluetooth_uart.tx_pin = 10;
		bluetooth_uart.port_num = 2;
	}
	else if(bluetooth_uart.LPC_uart == LPC_UART3)
	{
		bluetooth_uart.rx_pin = 1;
		bluetooth_uart.tx_pin = 0;
		bluetooth_uart.port_num = 3;
	}

	bluetooth_pin_config();
	bluetooth_uart_config();
	bluetooth_rx_dma_config();

	dma_add_handler(bluetooth_dma.rx_channel, bluetooth_rx_handler);

	GPDMA_ChannelCmd(bluetooth_dma.rx_channel, ENABLE);
}

int bluetooh_send_package(bluetooth_package_type type, void *data)
{
	bluetooth_package* package = malloc(sizeof(bluetooth_package));

	package->data[0] = '\0';

	switch(type)
	{
		case BT_PACKAGE_WEIGHT:
		case BT_PACKAGE_WATERLEVEL:
			int_to_ascii_str(*(int32_t*)data, package->data);

			break;
		case BT_PACKAGE_RELOAD:
			int_to_ascii_str((int)(*(uint16_t*)data), package->data);

			break;
		case BT_PACKAGE_PLAIN_TEXT:
		case BT_PACKAGE_UNITS:
		case BT_PACKAGE_GATE_STATUS:
			strcpy(package->data, (char*)data);

			break;
		case BT_PACKAGE_TARA:
		case BT_PACKAGE_SCALE:
			strcpy(package->data, "*");

			break;
		case BT_PACKAGE_KEYBOARD_INPUT:
		case BT_PACKAGE_KEYBOARD_MODE:
			package->data[0] = *(char*)data;
			package->data[1] = '\0';

			break;
		case BT_PACKAGE_ERROR_BAD_REFERENCE:
		case BT_PACKAGE_ERROR_BAD_RELOAD:
		case BT_PACKAGE_KEYBOARD_DELETE:
		case BT_PACKAGE_KEYBOARD_CANCEL:
			package->data[0] = '-';
			package->data[1] = '\0';

			break;
		default:
			free(package);

			return 1;
	}

	package->type = type;

	fill_str(package->data, '\0', BLUETOOTH_PACKAGE_SIZE);

	bluetooh_put_package(tx_queue, package);
	bluetooh_dma_start_tx();

	return 0;
}

int bluetooth_read_package(bluetooth_package_type* type, char* buffer)
{
	if(!*rx_queue)
		return 0;

	bluetooth_package aux;

	bluetooh_pop_package(rx_queue, &aux);

	*type = aux.type;

	strcpy(buffer, aux.data);

	return 1;
}

/** ------------ PUBLIC FUNCTIONS ------------ **/
