/*
 * radio.h
 *
 *  Created on: 27 kwi 2021
 *      Author: Dominik
 */

#ifndef RADIO_COMMUNICATION_RADIO_H_
#define RADIO_COMMUNICATION_RADIO_H_

// adresy urzadzen
#define MASTER 0x01
#define GARAGE_1_ADDRESS 0x02
#define MY_ADDRESS 0x03

#define MAX_DATA_BYTES 15 // maksymalna liczba bajtów danych w ramce

void send_radio_frame(uint8_t *data_bytes, uint8_t size, uint8_t address, uint8_t command);
void get_radio_frame(uint8_t *data_frame, uint8_t *RADIO_FLAG);


#endif /* RADIO_COMMUNICATION_RADIO_H_ */
