#ifndef __PETFEEDER_H__
#define __PETFEEDER_H__

#include "LPC17xx.h"

#include "lpc17xx_timer.h"

#include "WaterSensor.h"
#include "LoadCell.h"
#include "TIMHandler.h"
#include "Bluetooth.h"
#include "Keyboard.h"
#include "StepMotor.h"

#define LOADCELL_PORT 					(uint8_t)0
#define LOADCELL_DT_PIN 				(uint8_t)0
#define LOADCELL_SCK_PIN 				(uint8_t)1
#define LOADCELL_GAIN					(uint8_t)1

#define BLUETOOTH_DMA_RX_CHANNEL 		(uint8_t)1

#define KEYBOARD_PORT 					(uint8_t)2
#define KEYBOARD_INPUT_BUFFER 			(uint8_t)6

const uint8_t KEYBOARD_COLUMNS_PIN[4] = {4, 3, 2, 1};
const uint8_t KEYBOARD_ROWS_PIN[4]    = {8, 7, 6, 5};

#define STEPPMOTOR_PORT 				(uint8_t)0
#define STEPPMOTOR_IN1 					(uint8_t)9
#define STEPPMOTOR_IN2 					(uint8_t)8
#define STEPPMOTOR_IN3 					(uint8_t)7
#define STEPPMOTOR_IN4 					(uint8_t)6

#define WATERSENSOR_ADC_CHANNEL			(ADC_CHANNEL_SELECTION)0

#define SAMPLING_TO_TARE 				(uint8_t)50
#define SAMPLING_TO_CALIBRATE			(uint8_t)50
#define RELAOAD_UMBRAL					(uint8_t)10

uint8_t send_units_flag;
uint8_t send_gate_state_flag;
uint8_t send_reload_weigth_flag;
uint8_t in_input_reference_flag;
uint8_t in_input_reload_flag;
uint8_t change_gate_state_flag;
uint8_t open_gate_flag;

uint16_t reload_weigth;

void init(void);

void config_control_timer(void);

void control_steppmotor(void);
void control_bluetooh_rx(void);
void control_keyboard(void);
void control_timer(void);

void reload_control(int32_t weight);
void manual_calibration(char* input, uint8_t* index);
void manual_reload_weigth(char* input, uint8_t* index);

#endif /* __PETFEEDER_H__ */
