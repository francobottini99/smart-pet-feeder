#ifndef __LOADCELL_H__
#define __LOADCELL_H__

#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_clkpwr.h"
#include "Utils.h"

#include "TIMHandler.h"

#define LOADCELL_SAMPLES_PROM (uint8_t)10

void loadcell_init(uint8_t port, uint8_t dt_pin, uint8_t sck_pin, uint8_t gain, LPC_TIM_TypeDef *timer);
void loadcell_start_measuring(void);
void loadcell_start_tare(uint16_t n);
void loadcell_start_calibrate(uint16_t n, uint16_t reference);
int is_loadcell_in_tare(void);
int is_loadcell_in_scale(void);
int is_loadcell_set_scale(void);
int32_t loadcell_read(void);

#endif /* __LOADCELL_H__ */
