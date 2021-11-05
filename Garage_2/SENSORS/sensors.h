/*
 * sensors.h
 *
 *  Created on: 28 kwi 2021
 *      Author: Dominik
 */

#ifndef SENSORS_SENSORS_H_
#define SENSORS_SENSORS_H_

#define MOTION_SENSOR_1_DDR DDRC
#define MOTION_SENSOR_2_DDR DDRC
#define MOTION_SENSOR_1_PORT PINC
#define MOTION_SENSOR_2_PORT PINC
#define MOTION_SENSOR_1 (1<<PC3)
#define MOTION_SENSOR_2 (1<<PC4)

#define ECHO_1_DDR DDRC
#define ECHO_2_DDR DDRC
#define ECHO_3_DDR DDRC
#define ECHO_1_PIN (1<<PC5)
#define ECHO_2_PIN (1<<PC6)
#define ECHO_3_PIN (1<<PC7)

#define GATE_1_SENSOR_DDR DDRD
#define GATE_2_SENSOR_DDR DDRD
#define GATE_3_SENSOR_DDR DDRD
#define GATE_1_SENSOR_PORT PORTD
#define GATE_2_SENSOR_PORT PORTD
#define GATE_3_SENSOR_PORT PORTD
#define GATE_1_SENSOR (1<<PD5)
#define GATE_2_SENSOR (1<<PD6)
#define GATE_3_SENSOR (1<<PD7)

#define TRIG_1_ON GATE_1_SENSOR_PORT |= GATE_1_SENSOR
#define TRIG_2_ON GATE_2_SENSOR_PORT |= GATE_2_SENSOR
#define TRIG_3_ON GATE_3_SENSOR_PORT |= GATE_3_SENSOR
#define TRIG_1_OFF GATE_1_SENSOR_PORT &= ~GATE_1_SENSOR
#define TRIG_2_OFF GATE_2_SENSOR_PORT &= ~GATE_2_SENSOR
#define TRIG_3_OFF GATE_3_SENSOR_PORT &= ~GATE_3_SENSOR

#define IS_MOTION_1 (MOTION_SENSOR_1_PORT & MOTION_SENSOR_1)
#define IS_MOTION_2 (MOTION_SENSOR_2_PORT & MOTION_SENSOR_2)
#define GATE_1_IS_OPEN (MOTION_SENSOR_1_PORT & GATE_1_SENSOR)
#define GATE_2_IS_OPEN (MOTION_SENSOR_2_PORT & GATE_2_SENSOR)

// dotyczy czujnikow odleglosci !!
#define READY_TO_MEASURE_1 measure_flag & (1<<0)
#define READY_TO_MEASURE_2 measure_flag & (1<<1)
#define READY_TO_MEASURE_3 measure_flag & (1<<2)
#define MEAS_1_COMPLETED measure_flag & (1<<3)
#define MEAS_2_COMPLETED measure_flag & (1<<4)
#define MEAS_3_COMPLETED measure_flag & (1<<5)

void sensors_init(void);
uint16_t calculate_distance(uint16_t cnt_1, uint16_t cnt_2);
uint16_t measure(uint8_t kanal);

#endif /* SENSORS_SENSORS_H_ */
