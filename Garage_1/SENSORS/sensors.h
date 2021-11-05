/*
 * sensors.h
 *
 *  Created on: 28 kwi 2021
 *      Author: Dominik
 */

#ifndef SENSORS_SENSORS_H_
#define SENSORS_SENSORS_H_

#define MOTION_SENSOR_1_DDR DDRD
#define MOTION_SENSOR_1_PORT PIND
#define MOTION_SENSOR_1 (1<<PD5)

#define GATE_1_SENSOR_DDR DDRC
#define GATE_2_SENSOR_DDR DDRC
#define GATE_1_SENSOR_PORT PORTC
#define GATE_2_SENSOR_PORT PORTC
#define GATE_1_SENSOR (1<<PC6)
#define GATE_2_SENSOR (1<<PC7)

#define ECHO_1_DDR DDRD
#define ECHO_2_DDR DDRD
#define ECHO_1_PIN (1<<PD7)
#define ECHO_2_PIN (1<<PD6)

#define TRIG_1_ON GATE_1_SENSOR_PORT |= GATE_1_SENSOR
#define TRIG_2_ON GATE_2_SENSOR_PORT |= GATE_2_SENSOR
#define TRIG_1_OFF GATE_1_SENSOR_PORT &= ~GATE_1_SENSOR
#define TRIG_2_OFF GATE_2_SENSOR_PORT &= ~GATE_2_SENSOR

#define IS_MOTION_1 (MOTION_SENSOR_1_PORT & MOTION_SENSOR_1)
#define GATE_1_IS_OPEN (GATE_1_SENSOR_PORT & GATE_1_SENSOR)
#define GATE_2_IS_OPEN (GATE_2_SENSOR_PORT & GATE_2_SENSOR)

// dotyczy czujnikow odleglosci !!
#define READY_TO_MEASURE_1 measure_flag & (1<<0)
#define READY_TO_MEASURE_2 measure_flag & (1<<1)
#define MEAS_1_COMPLETED measure_flag & (1<<2)
#define MEAS_2_COMPLETED measure_flag & (1<<3)

void sensors_init(void);
uint16_t calculate_distance(uint16_t cnt_1, uint16_t cnt_2);
uint16_t measure(uint8_t kanal);

#endif /* SENSORS_SENSORS_H_ */
