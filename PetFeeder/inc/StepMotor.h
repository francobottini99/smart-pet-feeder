#ifndef __STEPP_MOTOR__
#define __STEPP_MOTOR__

#include "LPC17xx.h"

#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"

#include "Utils.h"
#include "TIMHandler.h"

typedef enum
{
	STEPOF45DEG  = (uint16_t)512,
    STEPOF90DEG  = (uint16_t)1024,
	STEPOF135DEG = (uint16_t)1536,
    STEPOF180DEG = (uint16_t)2048,
    STEPOF225DEG = (uint16_t)2560,
	STEPOF270DEG = (uint16_t)3072,
	STEPOF315DEG = (uint16_t)3584,
	STEPOF360DEG = (uint16_t)4096
} stepmotor_degree_rotation;

typedef enum
{
	ANTICLOCKWISE = (int8_t)-1,
	CLOCKWISE 	  = (int8_t)1
} stepmotor_direction;

void stepmotor_init(uint8_t port, uint8_t IN1, uint8_t IN2, uint8_t IN3, uint8_t IN4, LPC_TIM_TypeDef *timer);
void stepmotor_start_move(stepmotor_degree_rotation rotation, stepmotor_direction direction);
int is_stepmotor_in_rotation(void);

#endif  // __STEPP_MOTOR__
