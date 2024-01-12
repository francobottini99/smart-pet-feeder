#include "../inc/StepMotor.h"

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
} stepmotor_pin;

struct
{
	LPC_TIM_TypeDef *LPC_timer;

	TIM_TIMERCFG_Type config;
	TIM_MATCHCFG_Type match0;
} stepmotor_timer;

int16_t rotation;
int8_t direction;
int8_t aux_index;

uint8_t in_rotation_flag;

void stepmotor_config_pin(void);
void stepmotor_config_timer(void);
void stepmotor_move(void);

void stepmotor_move(void)
{
	for(uint8_t j = 0; j < 4; j++)
	{
		if(steps_control[aux_index][j])
			GPIO_SetValue(stepmotor_pin.port_num, 1<<stepmotor_pin.IN[j]);
		else
			GPIO_ClearValue(stepmotor_pin.port_num, 1<<stepmotor_pin.IN[j]);
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

		TIM_Cmd(stepmotor_timer.LPC_timer, DISABLE);
		TIM_ResetCounter(stepmotor_timer.LPC_timer);

		for(uint8_t j = 0; j < 4; j++)
			GPIO_ClearValue(stepmotor_pin.port_num, 1<<stepmotor_pin.IN[j]);
	}

	TIM_ClearIntPending(stepmotor_timer.LPC_timer, TIM_MR0_INT);
}

void stepmotor_config_pin(void)
{
	stepmotor_pin.pinsel.Portnum 	= stepmotor_pin.port_num;
	stepmotor_pin.pinsel.OpenDrain = PINSEL_PINMODE_NORMAL;
	stepmotor_pin.pinsel.Funcnum	= PINSEL_FUNC_0;
	stepmotor_pin.pinsel.Pinmode 	= PINSEL_PINMODE_TRISTATE;

	for(uint8_t i = 0; i < 4; i++)
	{
		stepmotor_pin.pinsel.Pinnum = stepmotor_pin.IN[i];
		PINSEL_ConfigPin(&stepmotor_pin.pinsel);
		GPIO_SetDir(stepmotor_pin.port_num, 1<<stepmotor_pin.IN[i], PINMODE_OUTPUT);
		GPIO_ClearValue(stepmotor_pin.port_num, 1<<stepmotor_pin.IN[i]);
	}
}

void stepmotor_config_timer(void)
{
	stepmotor_timer.config.PrescaleOption		=	TIM_PRESCALE_USVAL;
	stepmotor_timer.config.PrescaleValue		=	100;

	stepmotor_timer.match0.MatchChannel		=	0;
	stepmotor_timer.match0.IntOnMatch			=	ENABLE;
	stepmotor_timer.match0.ResetOnMatch		=	ENABLE;
	stepmotor_timer.match0.StopOnMatch			=	DISABLE;
	stepmotor_timer.match0.ExtMatchOutputType	=	TIM_EXTMATCH_NOTHING;
	stepmotor_timer.match0.MatchValue			=	10;

	TIM_Init(stepmotor_timer.LPC_timer, TIM_TIMER_MODE, &stepmotor_timer.config);

	TIM_ConfigMatch(stepmotor_timer.LPC_timer, &stepmotor_timer.match0);

	timer_add_handler(stepmotor_timer.LPC_timer, stepmotor_move);
}

void stepmotor_init(uint8_t port, uint8_t IN1, uint8_t IN2, uint8_t IN3, uint8_t IN4, LPC_TIM_TypeDef *timer)
{
	stepmotor_pin.port_num = port;

	stepmotor_pin.IN[0] = IN1;
	stepmotor_pin.IN[1] = IN2;
	stepmotor_pin.IN[2] = IN3;
	stepmotor_pin.IN[3] = IN4;

	stepmotor_timer.LPC_timer = timer;

	in_rotation_flag = 0;
	rotation = -1;
	direction = -1;
	aux_index = -1;

	stepmotor_config_pin();
	stepmotor_config_timer();
}

void stepmotor_start_move(stepmotor_degree_rotation rot, stepmotor_direction dir)
{
	if(!in_rotation_flag)
	{
		in_rotation_flag = 1;

		rotation = (int16_t)rot;
		direction = (int8_t)dir;

		aux_index = (direction > 0)? 0 : 7;

		TIM_Cmd(stepmotor_timer.LPC_timer, ENABLE);
	}
}

int is_stepmotor_in_rotation(void)
{
	return in_rotation_flag;
}
