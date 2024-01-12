#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "Utils.h"

void keyboard_init(uint8_t port, uint8_t pin_column[4], uint8_t pin_row[4], LPC_TIM_TypeDef *timer);
int8_t keyboard_get_keypress(void);

#endif /* __KEYBOARD_H__ */
