/*
 * sensors.c
 *
 *  Created on: 28 kwi 2021
 *      Author: Dominik
 */
#include <avr/io.h>
#include "sensors.h"

void sensors_init(void)
{
	ECHO_1_DDR &= ~ECHO_1_PIN;
	ECHO_2_DDR &= ~ECHO_2_PIN;
	ECHO_3_DDR &= ~ECHO_3_PIN;

	MOTION_SENSOR_1_DDR &= ~MOTION_SENSOR_1;
	MOTION_SENSOR_2_DDR &= ~MOTION_SENSOR_2;

	GATE_1_SENSOR_DDR |= GATE_1_SENSOR;
	GATE_2_SENSOR_DDR |= GATE_2_SENSOR;
	GATE_3_SENSOR_DDR |= GATE_3_SENSOR;
}
uint16_t calculate_distance(uint16_t cnt_1, uint16_t cnt_2)
{
	uint16_t value;
	value = cnt_2/2;
	value <<= 8;
	value |= cnt_1/2;
	return value;
}
uint16_t measure(uint8_t kanal)
{
	ADMUX = ADMUX & 0b11100000;
	ADMUX = ADMUX | kanal;
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	return ADCW;
}
