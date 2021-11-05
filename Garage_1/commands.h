/*
 * commands.h
 *
 *  Created on: 28 kwi 2021
 *      Author: Dominik
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

// liczba polecen w zwiazku z przesy³anymi danymi
#define NUM_OF_COMMANDS 5

// lista polecen
#define S1_S2_DATA_2_MASTER 1
#define S2_DATA_2_S1 2





void execute_command(uint8_t *data_frame);


#endif /* COMMANDS_H_ */
