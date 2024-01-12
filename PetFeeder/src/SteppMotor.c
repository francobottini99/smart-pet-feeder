#include "../inc/SteppMotor.h"

const uint8_t steps_control[8][4] =
{
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

struct
{
	PINSEL_CFG_Type pinsel;

	uint8_t port_num;
	uint8_t reserved[3];

	uint8_t IN[4];
} steppmotor_pin;

struct
{
	LPC_TIM_TypeDef *LPC_timer;

	TIM_TIMERCFG_Type config;
	TIM_MATCHCFG_Type match0;
} steppmotor_timer;

int16_t rotation;
int8_t direction;
int8_t aux_index;

uint8_t in_rotation_flag;

void steppmotor_config_pin(void);
void steppmotor_config_timer(void);
void steppmotor_move(void);

void steppmotor_move(void)
{
	for(uint8_t j = 0; j < 4; j++)
	{
		if(steps_control[aux_index][j])
			GPIO_SetValue(steppmotor_pin.port_num, 1<<steppmotor_pin.IN[j]);
		else
			GPIO_ClearValue(steppmotor_pin.port_num, 1<<steppmotor_pin.IN[j]);
	}

	aux_index += direction;

	aux_index = (aux_index + 8) % 8;

	if(rotation > 0)
		rotation--;
	else
	{
		in_rotation_flag = 0;

		rotation = -1;
		direction = -1;
		aux_index = -1;

		TIM_Cmd(steppmotor_timer.LPC_timer, DISABLE);
		TIM_ResetCounter(steppmotor_timer.LPC_timer);

		for(uint8_t j = 0; j < 4; j++)
			GPIO_ClearValue(steppmotor_pin.port_num, 1<<steppmotor_pin.IN[j]);
	}

	TIM_ClearIntPending(steppmotor_timer.LPC_timer, TIM_MR0_INT);
}

void steppmotor_config_pin(void)
{
	steppmotor_pin.pinsel.Portnum 	= steppmotor_pin.port_num;
	steppmotor_pin.pinsel.OpenDrain = PINSEL_PINMODE_NORMAL;
	steppmotor_pin.pinsel.Funcnum	= PINSEL_FUNC_0;
	steppmotor_pin.pinsel.Pinmode 	= PINSEL_PINMODE_TRISTATE;

	for(uint8_t i = 0; i < 4; i++)
	{
		steppmotor_pin.pinsel.Pinnum = steppmotor_pin.IN[i];
		PINSEL_ConfigPin(&steppmotor_pin.pinsel);
		GPIO_SetDir(steppmotor_pin.port_num, 1<<steppmotor_pin.IN[i], PINMODE_OUTPUT);
		GPIO_ClearValue(steppmotor_pin.port_num, 1<<steppmotor_pin.IN[i]);
	}
}

void steppmotor_config_timer(void)
{
	steppmotor_timer.config.PrescaleOption		=	TIM_PRESCALE_USVAL;
	steppmotor_timer.config.PrescaleValue		=	100;

	steppmotor_timer.match0.MatchChannel		=	0;
	steppmotor_timer.match0.IntOnMatch			=	ENABLE;
	steppmotor_timer.match0.ResetOnMatch		=	ENABLE;
	steppmotor_timer.match0.StopOnMatch			=	DISABLE;
	steppmotor_timer.match0.ExtMatchOutputType	=	TIM_EXTMATCH_NOTHING;
	steppmotor_timer.match0.MatchValue			=	10;

	TIM_Init(steppmotor_timer.LPC_timer, TIM_TIMER_MODE, &steppmotor_timer.config);

	TIM_ConfigMatch(steppmotor_timer.LPC_timer, &steppmotor_timer.match0);

	timer_add_handler(steppmotor_timer.LPC_timer, steppmotor_move);
}

void steppmotor_init(uint8_t port, uint8_t IN1, uint8_t IN2, uint8_t IN3, uint8_t IN4, LPC_TIM_TypeDef *timer)
{
	steppmotor_pin.port_num = port;

	steppmotor_pin.IN[0] = IN1;
	steppmotor_pin.IN[1] = IN2;
	steppmotor_pin.IN[2] = IN3;
	steppmotor_pin.IN[3] = IN4;

	steppmotor_timer.LPC_timer = timer;

	in_rotation_flag = 0;
	rotation = -1;
	direction = -1;
	aux_index = -1;

	steppmotor_config_pin();
	steppmotor_config_timer();
}

void steppmotor_start_move(steppmotor_degree_rotation rot, steppmotor_direction dir)
{
	if(!in_rotation_flag)
	{
		in_rotation_flag = 1;

		rotation = (int16_t)rot;
		direction = (int8_t)dir;

		aux_index = (direction > 0)? 0 : 7;

		TIM_Cmd(steppmotor_timer.LPC_timer, ENABLE);
	}
}

int is_steppmotor_in_rotation(void)
{
	return in_rotation_flag;
}
