#include "../inc/Keyboard.h"

struct
{
	PINSEL_CFG_Type pinsel;

	uint8_t port_num;
	uint8_t reserved[3];
	uint8_t pin_row[4];
	uint8_t pin_column[4];
} keyboard_pin;

struct
{
	LPC_TIM_TypeDef *LPC_timer;

	TIM_TIMERCFG_Type config;
	TIM_MATCHCFG_Type match0;
} keyboard_timer;

int8_t column_press;
int8_t row_press;

void keyboard_config_pin(void);
void keyboard_timer_config(void);
void keyboard_scan(void);

void EINT3_IRQHandler()
{
    if (keyboard_pin.port_num == 2)
    	for(uint8_t i = 0; i < 4; i++)
    		LPC_GPIOINT->IO2IntEnR &= ~(1<<keyboard_pin.pin_column[i]);
    else if (keyboard_pin.port_num == 1)
    	for(uint8_t i = 0; i < 4; i++)
    		LPC_GPIOINT->IO0IntEnR &= ~(1<<keyboard_pin.pin_column[i]);

	TIM_Cmd(keyboard_timer.LPC_timer, ENABLE);
}

void keyboard_scan(void)
{
	row_press = -1;
	column_press = -1;

	for(uint8_t i = 0; i < 4; i++)
		GPIO_ClearValue(keyboard_pin.port_num, 1<<keyboard_pin.pin_row[i]);

	for(uint8_t i = 0; i < 4; i++)
	{
		GPIO_SetValue(keyboard_pin.port_num, 1<<keyboard_pin.pin_row[i]);

		for(uint8_t j = 0; j < 4; j++)
		{
			if((GPIO_ReadValue(keyboard_pin.port_num) & 1<<keyboard_pin.pin_column[j]))
			{
				column_press = j;
				row_press = i;
				break;
			}
		}

		if(row_press >= 0)
			break;
	}

	for(uint8_t i = 0; i < 4; i++)
	{
		GPIO_SetValue(keyboard_pin.port_num, 1<<keyboard_pin.pin_row[i]);
		GPIO_ClearInt(keyboard_pin.port_num, 1<<keyboard_pin.pin_column[i]);
		GPIO_IntCmd(keyboard_pin.port_num, 1<<keyboard_pin.pin_column[i], RISING_EDGE);
	}

	TIM_ClearIntPending(keyboard_timer.LPC_timer, TIM_MR0_INT);
}

void keyboard_config_pin(void)
{
	keyboard_pin.pinsel.Portnum 	= keyboard_pin.port_num;
	keyboard_pin.pinsel.OpenDrain 	= PINSEL_PINMODE_NORMAL;
	keyboard_pin.pinsel.Funcnum		= PINSEL_FUNC_0;

	for(uint8_t i = 0; i < 4; i++ )
	{
		keyboard_pin.pinsel.Pinnum 	= keyboard_pin.pin_column[i];
		keyboard_pin.pinsel.Pinmode = PINSEL_PINMODE_PULLDOWN;
		PINSEL_ConfigPin(&keyboard_pin.pinsel);
		GPIO_SetDir(keyboard_pin.port_num, 1<<keyboard_pin.pin_column[i], PINMODE_INPUT);
		GPIO_ClearInt(keyboard_pin.port_num, 1<<keyboard_pin.pin_column[i]);
		GPIO_IntCmd(keyboard_pin.port_num, 1<<keyboard_pin.pin_column[i], RISING_EDGE);

		keyboard_pin.pinsel.Pinnum 	= keyboard_pin.pin_row[i];
		keyboard_pin.pinsel.Pinmode = PINSEL_PINMODE_TRISTATE;
		PINSEL_ConfigPin(&keyboard_pin.pinsel);
		GPIO_SetDir(keyboard_pin.port_num, 1<<keyboard_pin.pin_row[i], PINMODE_OUTPUT);
		GPIO_SetValue(keyboard_pin.port_num, 1<<keyboard_pin.pin_row[i]);
	}

	NVIC_EnableIRQ(EINT3_IRQn);
}

void keyboard_init(uint8_t port, uint8_t pin_column[4], uint8_t pin_row[4], LPC_TIM_TypeDef *timer)
{
	keyboard_pin.port_num = port;

	for(uint8_t i = 0; i < 4; i++)
	{
		keyboard_pin.pin_row[i] = pin_row[i];
		keyboard_pin.pin_column[i] = pin_column[i];
	}

	keyboard_timer.LPC_timer = timer;

	row_press = -1;
	column_press = -1;

	keyboard_config_pin();
	keyboard_timer_config();
}

void keyboard_timer_config(void)
{
	keyboard_timer.config.PrescaleOption		=	TIM_PRESCALE_USVAL;
	keyboard_timer.config.PrescaleValue			=	100;

	keyboard_timer.match0.MatchChannel			=	0;
	keyboard_timer.match0.IntOnMatch			=	ENABLE;
	keyboard_timer.match0.ResetOnMatch			=	ENABLE;
	keyboard_timer.match0.StopOnMatch			=	ENABLE;
	keyboard_timer.match0.ExtMatchOutputType	=	TIM_EXTMATCH_NOTHING;
	keyboard_timer.match0.MatchValue			=	20;

	TIM_Init(keyboard_timer.LPC_timer, TIM_TIMER_MODE, &keyboard_timer.config);

	TIM_ConfigMatch(keyboard_timer.LPC_timer, &keyboard_timer.match0);

	timer_add_handler(keyboard_timer.LPC_timer, keyboard_scan);
}

int8_t keyboard_get_keypress(void)
{
	if(row_press >= 0 && column_press >= 0)
	{
		uint8_t key = (row_press<<2) + column_press;

		row_press = -1;
		column_press = -1;

		switch(key)
		{
			case 0:
			case 1:
			case 2:
				return key + 1 + '0';
			case 3:
				return 'A';
			case 4:
			case 5:
			case 6:
				return key + '0';
			case 7:
				return 'B';
			case 8:
			case 9:
			case 10:
				return key - 1 + '0';
			case 11:
				return 'C';
			case 12:
				return '*';
			case 13:
				return '0';
			case 14:
				return '#';
			case 15:
				return 'D';
		}
	}

	return -1;
}
