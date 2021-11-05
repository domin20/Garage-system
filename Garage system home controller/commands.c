/*
 * commands.c
 *
 *  Created on: 28 kwi 2021
 *      Author: Dominik
 */
#include "main.h"
#include "commands.h"
#include "SPI_lib.h"

extern uint8_t state_sensors_1;
extern int8_t temperature_1;
extern uint8_t state_sensors_2;
extern int8_t temperature_2;
extern uint8_t register_1, register_2;

void execute_command(uint8_t *data_frame)
{
	switch(data_frame[2])
	{
	case 1: state_sensors_1 = data_frame[4];
			state_sensors_2 = data_frame[5];
			temperature_1 = data_frame[6];
			temperature_2 = data_frame[7];
			register_2 = (state_sensors_2 & 0b11111) + ((state_sensors_1 & 0x02)<<5);
			register_1 = ((state_sensors_1 & 0x04)>>2) + ((state_sensors_1 & 0x01)<<1);
			SendSoftSpi(register_1);
			SendSoftSpi2(register_2);
			break;
	}
}

