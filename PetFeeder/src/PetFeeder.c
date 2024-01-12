#include "../inc/PetFeeder.h"

int main(void)
{
	init();

	while(1)
	{
		control_steppmotor();
		control_bluetooh_rx();
		control_keyboard();
	}

    return 0 ;
}

void init(void)
{
	send_units_flag 		= 1;
	send_gate_state_flag 	= 1;
	send_reload_weigth_flag = 1;
	open_gate_flag 			= 0;
	in_input_reference_flag = 0;
	in_input_reload_flag	= 0;
	change_gate_state_flag 	= 0;
	reload_weigth 			= 0;

	GPDMA_Init();

	bluetooth_init(LPC_UART0, BLUETOOTH_DMA_RX_CHANNEL);
	loadcell_init(LOADCELL_PORT, LOADCELL_DT_PIN, LOADCELL_SCK_PIN, LOADCELL_GAIN, LPC_TIM2);
	keyboard_init(KEYBOARD_PORT, KEYBOARD_COLUMNS_PIN, KEYBOARD_ROWS_PIN, LPC_TIM0);
	stepmotor_init(STEPPMOTOR_PORT, STEPPMOTOR_IN1, STEPPMOTOR_IN2, STEPPMOTOR_IN3, STEPPMOTOR_IN4, LPC_TIM3);
	watersensor_init(WATERSENSOR_ADC_CHANNEL);

	bluetooh_send_package(BT_PACKAGE_KEYBOARD_CANCEL, NULL);

	config_control_timer();
}

void config_control_timer(void)
{
	TIM_TIMERCFG_Type timer_config;
	TIM_MATCHCFG_Type timer_match_config;

	timer_config.PrescaleOption		=	TIM_PRESCALE_USVAL;
	timer_config.PrescaleValue		=	1000;

	timer_match_config.MatchChannel			=	0;
	timer_match_config.IntOnMatch			=	ENABLE;
	timer_match_config.ResetOnMatch			=	ENABLE;
	timer_match_config.StopOnMatch			=	DISABLE;
	timer_match_config.ExtMatchOutputType	=	TIM_EXTMATCH_NOTHING;
	timer_match_config.MatchValue			= 	100;

	TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &timer_config);

	TIM_ConfigMatch(LPC_TIM1, &timer_match_config);

	timer_add_handler(LPC_TIM1, control_timer);

	TIM_Cmd(LPC_TIM1, ENABLE);
}

void control_steppmotor(void)
{
	if(is_stepmotor_in_rotation())
		return;

	if(send_gate_state_flag)
	{
		bluetooh_send_package(BT_PACKAGE_GATE_STATUS, open_gate_flag? "OPEN" : "CLOSE");
		send_gate_state_flag = 0;
	}

	if(change_gate_state_flag)
	{
		if(open_gate_flag)
			stepmotor_start_move(STEPOF135DEG, CLOCKWISE);
		else
			stepmotor_start_move(STEPOF135DEG, ANTICLOCKWISE);

		open_gate_flag = !open_gate_flag;
		change_gate_state_flag = 0;
		send_gate_state_flag = 1;

		bluetooh_send_package(BT_PACKAGE_GATE_STATUS, "*");
	}
}

void control_bluetooh_rx(void)
{
	uint16_t reference;
	char bluetooth_buffer[BLUETOOTH_PACKAGE_SIZE];
	bluetooth_package_type type;

	if(bluetooth_read_package(&type, bluetooth_buffer))
	{
		switch(type)
		{
			case BT_PACKAGE_TARA:
				loadcell_start_tare(SAMPLING_TO_TARE);

				break;
			case BT_PACKAGE_SCALE:;
				uint16_t reference = (uint16_t)atoi(bluetooth_buffer);

				if(reference > 0)
					loadcell_start_calibrate(SAMPLING_TO_CALIBRATE, reference);
				else
					bluetooh_send_package(BT_PACKAGE_ERROR_BAD_REFERENCE, NULL);

				break;
			case BT_PACKAGE_RELOAD:;
				uint16_t weigth = (uint16_t)atoi(bluetooth_buffer);

				if(weigth > RELAOAD_UMBRAL)
				{
					reload_weigth = weigth;
					send_reload_weigth_flag = 1;
				}
				else
					bluetooh_send_package(BT_PACKAGE_ERROR_BAD_RELOAD, NULL);

				break;
			case BT_PACKAGE_STATUS_CONFIG:
				send_units_flag = 1;
				send_gate_state_flag = 1;
				send_reload_weigth_flag = 1;

				break;
			default:
				break;
		}
	}
}

void control_keyboard(void)
{
	static uint8_t keyboard_buffer_index = 0;
	static char keyboard_input_buffer[KEYBOARD_INPUT_BUFFER];

	int8_t keypress = keyboard_get_keypress();

	if(keypress > 0)
	{
		switch(keypress)
		{
			case '*':
				if(!in_input_reference_flag)
					loadcell_start_tare(SAMPLING_TO_TARE);

				break;
			case '#':
				if(in_input_reload_flag)
					break;

				manual_calibration(keyboard_input_buffer, &keyboard_buffer_index);

				bluetooh_send_package(BT_PACKAGE_KEYBOARD_MODE, &keypress);

				break;
			case 'D':
				if(in_input_reference_flag || in_input_reload_flag)
					keyboard_buffer_index = 0;

				bluetooh_send_package(BT_PACKAGE_KEYBOARD_DELETE, NULL);

				break;
			case 'A':
				if(in_input_reference_flag)
					break;

				manual_reload_weigth(keyboard_input_buffer, &keyboard_buffer_index);

				bluetooh_send_package(BT_PACKAGE_KEYBOARD_MODE, &keypress);

				break;
			case 'B':
				change_gate_state_flag = 1;

				break;
			case 'C':
				if(in_input_reference_flag || in_input_reload_flag)
				{
					keyboard_buffer_index = 0;
					in_input_reference_flag = 0;
					in_input_reload_flag = 0;

					bluetooh_send_package(BT_PACKAGE_KEYBOARD_CANCEL, NULL);
				}

				break;
			default:
				if(in_input_reference_flag || in_input_reload_flag)
				{
					keyboard_input_buffer[keyboard_buffer_index] = keypress;

					if(keyboard_buffer_index < KEYBOARD_INPUT_BUFFER - 1)
						keyboard_buffer_index++;
					else
						keyboard_buffer_index = 0;
				}

				bluetooh_send_package(BT_PACKAGE_KEYBOARD_INPUT, &keypress);

				break;
		}
	}
}

void control_timer(void)
{
	static uint8_t bt_send_counter = 0;

	if(bt_send_counter < 10)
		bt_send_counter++;
	else
	{
		int32_t weight = loadcell_read();
		uint32_t water = watersensor_read();

		if(is_loadcell_in_tare())
			bluetooh_send_package(BT_PACKAGE_TARA, NULL);
		else if(is_loadcell_in_scale())
		{
			bluetooh_send_package(BT_PACKAGE_SCALE, NULL);
			send_units_flag = 1;
		}
		else if(send_units_flag)
		{
			bluetooh_send_package(BT_PACKAGE_UNITS, is_loadcell_set_scale()? "YES" : "NO");
			send_units_flag = 0;
		}
		else
			bluetooh_send_package(BT_PACKAGE_WEIGHT, &weight);

		reload_control(weight);

		bluetooh_send_package(BT_PACKAGE_WATERLEVEL, &water);

		bt_send_counter = 0;
	}

	if(send_reload_weigth_flag)
	{
		bluetooh_send_package(BT_PACKAGE_RELOAD, &reload_weigth);
		send_reload_weigth_flag = 0;
	}

	loadcell_start_measuring();
	watersensor_start_measuring();

	TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);
}

void reload_control(int32_t weight)
{
	if(is_loadcell_set_scale())
	{
		if(reload_weigth > RELAOAD_UMBRAL)
		{
			if(!open_gate_flag)
			{
				if(weight < RELAOAD_UMBRAL)
					change_gate_state_flag = 1;
			}
			else if(open_gate_flag)
			{
				if(weight > reload_weigth - RELAOAD_UMBRAL)
					change_gate_state_flag = 1;
			}
		}
	}
}

void manual_calibration(char* input, uint8_t* index)
{
	uint16_t reference = 0;

	if(!in_input_reference_flag)
		in_input_reference_flag = 1;
	else
	{
		in_input_reference_flag = 0;

		input[*index] = '\0';

		reference = (uint16_t)atoi(input);

		if(*index > 0 && reference > 0)
			loadcell_start_calibrate(SAMPLING_TO_CALIBRATE, reference);
		else
			bluetooh_send_package(BT_PACKAGE_ERROR_BAD_REFERENCE, NULL);

		*index = 0;
	}
}

void manual_reload_weigth(char* input, uint8_t* index)
{
	if(!in_input_reload_flag)
		in_input_reload_flag = 1;
	else
	{
		in_input_reload_flag = 0;

		input[*index] = '\0';

		reload_weigth = (uint16_t)atoi(input);

		if(*index == 0 || reload_weigth <= RELAOAD_UMBRAL)
		{
			reload_weigth = 0;
			bluetooh_send_package(BT_PACKAGE_ERROR_BAD_RELOAD, NULL);
		}
		else
			send_reload_weigth_flag = 1;

		*index = 0;
	}
}
