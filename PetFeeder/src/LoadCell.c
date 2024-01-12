#include "../inc/LoadCell.h"

struct
{
	LPC_TIM_TypeDef *LPC_timer;

	TIM_TIMERCFG_Type config;
	TIM_MATCHCFG_Type match0;
} loadcell_timer;

struct
{
	PINSEL_CFG_Type dt_pinsel_config;
	PINSEL_CFG_Type sck_pinsel_config;

	uint8_t port;
	uint8_t dt_pin;
	uint8_t sck_pin;
	uint8_t reserved;
} loadcell_pin;

struct
{
	int32_t array[LOADCELL_SAMPLES_PROM];
	int32_t last_prom;
	int32_t last_value;

	int16_t scale_reference;

	int32_t n_sampling;
	int64_t acumulator;

	int32_t tare;
	int32_t scale;

	uint8_t in_scale_flag;
	uint8_t in_tare_flag;
	uint8_t in_read_flag;

	uint8_t reserved[3];
} loadcell_measuring;

uint8_t loadcell_gain;

static uint8_t i = 0;

void loadcell_measuring_handler(void);
void loadcell_pin_config(void);
void loadcell_timer_config(void);
int  loadcell_is_ready(void);
void loadcell_sck_pin_toggle(void);
void loadcell_calculate_tare(void);
void loadcell_calculate_scale(void);

/** ------------ HANDLER LOADCELL ------------ **/

void loadcell_measuring_handler(void)
{
	static uint8_t toggles = 0;
	static uint8_t pulses = 0;

	loadcell_sck_pin_toggle();

	toggles++;

	if(toggles == 48 + 2 * loadcell_gain)
	{
		TIM_Cmd(loadcell_timer.LPC_timer, DISABLE);
		TIM_ResetCounter(loadcell_timer.LPC_timer);

		pulses = 0;
		toggles = 0;

		loadcell_measuring.in_read_flag = 0;

		loadcell_measuring.last_value = loadcell_measuring.array[i];

		if(i < LOADCELL_SAMPLES_PROM)
			i++;
		else
			i = 0;

		if(loadcell_measuring.in_tare_flag)
			loadcell_calculate_tare();

		if(loadcell_measuring.in_scale_flag)
			loadcell_calculate_scale();
	}

	if(pulses < 24 && toggles % 2 == 0)
	{
		loadcell_measuring.array[i] |= (GPIO_ReadValue(loadcell_pin.port) & (1<<loadcell_pin.dt_pin))<<(23 - pulses);

		pulses++;
	}

	TIM_ClearIntPending(loadcell_timer.LPC_timer, TIM_MR0_INT);
}

int loadcell_is_ready(void)
{
	return (GPIO_ReadValue(loadcell_pin.port) & (1<<loadcell_pin.dt_pin)) == LOW;
}

void loadcell_sck_pin_toggle(void)
{
	if (GPIO_ReadValue(loadcell_pin.port) & (1<<loadcell_pin.sck_pin))
		GPIO_ClearValue(loadcell_pin.port, 1<<loadcell_pin.sck_pin);
	else
		GPIO_SetValue(loadcell_pin.port, 1<<loadcell_pin.sck_pin);
}

void loadcell_calculate_tare(void)
{
	static uint16_t counter = 0;

	if(counter < loadcell_measuring.n_sampling)
	{
		loadcell_measuring.acumulator += loadcell_measuring.last_value;
		counter++;
	}
	else
	{
		loadcell_measuring.tare = loadcell_measuring.acumulator / loadcell_measuring.n_sampling;
		loadcell_measuring.acumulator = 0;
		loadcell_measuring.n_sampling = 0;
		loadcell_measuring.in_tare_flag = 0;
		counter = 0;
	}
}

void loadcell_calculate_scale(void)
{
	static uint16_t counter = 0;

	if(counter < loadcell_measuring.n_sampling)
	{
		loadcell_measuring.acumulator += loadcell_measuring.last_value - loadcell_measuring.tare;
		counter++;
	}
	else
	{
		loadcell_measuring.scale = loadcell_measuring.acumulator / (loadcell_measuring.n_sampling * loadcell_measuring.scale_reference);
		loadcell_measuring.acumulator = 0;
		loadcell_measuring.n_sampling = 0;
		loadcell_measuring.in_scale_flag = 0;
		counter = 0;
	}
}

/** ------------ HANDLER LOADCELL ------------ **/

/** ------------ LOADCELL CONFIG ------------ **/

void loadcell_timer_config(void)
{
	loadcell_timer.config.PrescaleOption		=	TIM_PRESCALE_TICKVAL;
	loadcell_timer.config.PrescaleValue			=	1;

	loadcell_timer.match0.MatchChannel			=	0;
	loadcell_timer.match0.IntOnMatch			=	ENABLE;
	loadcell_timer.match0.ResetOnMatch			=	ENABLE;
	loadcell_timer.match0.StopOnMatch			=	DISABLE;
	loadcell_timer.match0.ExtMatchOutputType	=	TIM_EXTMATCH_NOTHING;
	loadcell_timer.match0.MatchValue			=	199;

	TIM_Init(loadcell_timer.LPC_timer, TIM_TIMER_MODE, &loadcell_timer.config);

	TIM_ConfigMatch(loadcell_timer.LPC_timer, &loadcell_timer.match0);

	timer_add_handler(loadcell_timer.LPC_timer, loadcell_measuring_handler);
}

void loadcell_pin_config(void)
{
	loadcell_pin.dt_pinsel_config.Portnum 	=	loadcell_pin.port;
	loadcell_pin.dt_pinsel_config.Pinnum	=	loadcell_pin.dt_pin;
	loadcell_pin.dt_pinsel_config.Pinmode	=	PINSEL_PINMODE_TRISTATE;
	loadcell_pin.dt_pinsel_config.Funcnum	= 	PINSEL_FUNC_0;
	loadcell_pin.dt_pinsel_config.OpenDrain	=	PINSEL_PINMODE_NORMAL;

	loadcell_pin.sck_pinsel_config.Portnum 	=	loadcell_pin.port;
	loadcell_pin.sck_pinsel_config.Pinnum	=	loadcell_pin.sck_pin;
	loadcell_pin.sck_pinsel_config.Funcnum	= 	PINSEL_FUNC_0;

	PINSEL_ConfigPin(&loadcell_pin.dt_pinsel_config);
	PINSEL_ConfigPin(&loadcell_pin.sck_pinsel_config);

	GPIO_SetDir(loadcell_pin.port, 1<<loadcell_pin.dt_pin, PINMODE_INPUT);
	GPIO_SetDir(loadcell_pin.port, 1<<loadcell_pin.sck_pin, PINMODE_OUTPUT);

	GPIO_ClearValue(loadcell_pin.port, 1<<loadcell_pin.sck_pin);
}

/** ------------ LOADCELL CONFIG ------------ **/

/** ------------ PUBLIC FUNCTIONS ------------ **/

void loadcell_init(uint8_t port, uint8_t dt_pin, uint8_t sck_pin, uint8_t gain, LPC_TIM_TypeDef *timer)
{
	loadcell_pin.port = port;
	loadcell_pin.dt_pin = dt_pin;
	loadcell_pin.sck_pin = sck_pin;
	loadcell_gain = gain;
	loadcell_timer.LPC_timer = timer;

	loadcell_measuring.last_prom = 0;
	loadcell_measuring.last_value = 0;

	loadcell_measuring.scale_reference = 1;
	loadcell_measuring.n_sampling = 0;
	loadcell_measuring.tare = 0;
	loadcell_measuring.scale = 1;
	loadcell_measuring.acumulator = 0;

	loadcell_measuring.in_read_flag = 0;
	loadcell_measuring.in_tare_flag = 0;
	loadcell_measuring.in_scale_flag = 0;

	for(uint8_t i = 0; i < LOADCELL_SAMPLES_PROM; i++)
		loadcell_measuring.array[i] = 0;

	loadcell_pin_config();
	loadcell_timer_config();
}

int32_t loadcell_read(void)
{
	if(!loadcell_measuring.in_read_flag)
	{
		uint32_t sum = 0;

		for(uint8_t i = 0; i < LOADCELL_SAMPLES_PROM; i++)
			sum += loadcell_measuring.array[i];

		loadcell_measuring.last_prom = sum / LOADCELL_SAMPLES_PROM;
	}

	return (loadcell_measuring.last_prom - loadcell_measuring.tare) / loadcell_measuring.scale;
}

void loadcell_start_tare(uint16_t n)
{
	if(!loadcell_measuring.in_tare_flag && !loadcell_measuring.in_scale_flag)
	{
		loadcell_measuring.n_sampling = n;
		loadcell_measuring.in_tare_flag = 1;
	}
}

void loadcell_start_calibrate(uint16_t n, uint16_t reference)
{
	if(!loadcell_measuring.in_scale_flag && !loadcell_measuring.in_tare_flag)
	{
		loadcell_measuring.scale_reference = reference;
		loadcell_measuring.n_sampling = n;
		loadcell_measuring.in_scale_flag = 1;
	}
}

int is_loadcell_in_tare(void)
{
	return loadcell_measuring.in_tare_flag;
}

int is_loadcell_in_scale(void)
{
	return loadcell_measuring.in_scale_flag;
}

int is_loadcell_set_scale(void)
{
	return (loadcell_measuring.scale == 1)? 0 : 1;
}

void loadcell_start_measuring(void)
{
	if(!loadcell_measuring.in_read_flag)
	{
		loadcell_measuring.in_read_flag = 1;

		loadcell_measuring.array[i] = 0;

		while(!loadcell_is_ready());

		TIM_Cmd(loadcell_timer.LPC_timer, ENABLE);
	}
}

/** ------------ PUBLIC FUNCTIONS ------------ **/
