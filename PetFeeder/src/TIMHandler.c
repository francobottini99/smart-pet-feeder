#include "../inc/TIMHandler.h"

struct
{
	void (*timer0)(void);
	void (*timer1)(void);
	void (*timer2)(void);
	void (*timer3)(void);
} timer_handlers = {NULL, NULL, NULL, NULL};

void TIMER0_IRQHandler(void)
{
	if(timer_handlers.timer0)
		(*timer_handlers.timer0)();
}

void TIMER1_IRQHandler(void)
{
	if(timer_handlers.timer1)
		(*timer_handlers.timer1)();
}

void TIMER2_IRQHandler(void)
{
	if(timer_handlers.timer2)
		(*timer_handlers.timer2)();
}

void TIMER3_IRQHandler(void)
{
	if(timer_handlers.timer3)
		(*timer_handlers.timer3)();
}

int timer_add_handler(LPC_TIM_TypeDef *LPC_timer, void (*handler)(void))
{
	if(LPC_timer == LPC_TIM0)
	{
		timer_handlers.timer0 = handler;
		NVIC_EnableIRQ(TIMER0_IRQn);
	}
	else if(LPC_timer == LPC_TIM1)
	{
		timer_handlers.timer1 = handler;
		NVIC_EnableIRQ(TIMER1_IRQn);
	}
	else if(LPC_timer == LPC_TIM2)
	{
		timer_handlers.timer2 = handler;
		NVIC_EnableIRQ(TIMER2_IRQn);
	}
	else if(LPC_timer == LPC_TIM3)
	{
		timer_handlers.timer3 = handler;
		NVIC_EnableIRQ(TIMER3_IRQn);
	}
	else
		return 1;

	return 0;
}

int timer_remove_handler(LPC_TIM_TypeDef *LPC_timer)
{
	if(LPC_timer == LPC_TIM0)
	{
		timer_handlers.timer0 = NULL;
		NVIC_DisableIRQ(TIMER0_IRQn);
	}
	else if(LPC_timer == LPC_TIM1)
	{
		timer_handlers.timer1 = NULL;
		NVIC_DisableIRQ(TIMER1_IRQn);
	}
	else if(LPC_timer == LPC_TIM2)
	{
		timer_handlers.timer2 = NULL;
		NVIC_DisableIRQ(TIMER2_IRQn);
	}
	else if(LPC_timer == LPC_TIM3)
	{
		timer_handlers.timer3 = NULL;
		NVIC_DisableIRQ(TIMER3_IRQn);
	}
	else
		return 1;

	return 0;
}
