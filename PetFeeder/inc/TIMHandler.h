#ifndef __TIMHANDLER_H__
#define __TIMHANDLER_H__

#include "lpc17xx_timer.h"

#include "Utils.h"

int timer_add_handler(LPC_TIM_TypeDef *LPC_timer, void (*handler)(void));
int timer_remove_handler(LPC_TIM_TypeDef *LPC_timer);

#endif /* __TIMHANDLER_H__ */
