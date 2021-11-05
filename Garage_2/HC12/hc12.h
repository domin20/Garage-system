/*
 * hc12.h
 *
 *  Created on: 17 wrz 2020
 *      Author: Dominik
 */

#ifndef HC12_HC12_H_
#define HC12_HC12_H_

// *****************  K O N F I G U R A C J A  *****************
// ******* U S T A W I E N I A     U Z Y T K O W N I K A *******


#define TRANSMITTER 1	// nadajnik
#define RECEIVER 1		// odbiornik

// dyrektywy do okreslania statusu ramki
#define FRAME_RESTART 0
#define FRAME_OK 1


// ZABEZPIECZENIE PRZESY£ANYCH DANYCH (kodowanie z wykorzystaniem RTC)
// 0 - WY£¥CZONE
// 1 - W£¥CZONE
// 2 - PRZESY£ANIE RAMKI O ROZMIARZE 1-10 BAJTOW DANYCH OPAKOWANYCH W BAJTY SPRAWDZAJACE
	// FORMAT:
	// 1 BAJT: BAJT SPRAWDZAJACY,
	// 2 BAJT: ROZMIAR,
	// 3-.. BAJTY DANYCH,
	// DWA OSTATNIE BAJTY: SUMA KONTROLNA

#define DATA_SECURITY 0

#if DATA_SECURITY == 2
#define MAX_DATA_BYTES 10	// maksymalna ilosc bajtow danych przy wysylaniu ramki
#endif
// 0 - sterowanie zasilaniem nadajnika/odbiornika WY£¥CZONE
// 1 - sterowanie zasilaniem nadajnika/odbiornika W£¥CZONE
#define USE_RADIO_PWR 0

#if USE_RADIO_PWR == 1
// wybieramy pin, którym bêdzie za³¹czane zasilanie nadajnika/odbiornika
#define RADIO_PIN (1<<PD3)
#define RADIO_PORT PORTD

#define RADIO_ON RADIO_PORT |= RADIO_PIN; _delay_ms(5)
#define RADIO_OFF RADIO_PORT &= ~RADIO_PIN
#endif

#if (TRANSMITTER == 1) || (RECEIVER == 1 && DATA_SECURITY == 1)
void send_radio_byte(uint8_t byte);
#endif

#if RECEIVER == 1
uint8_t get_radio_byte(void);
#endif

#if DATA_SECURITY == 1 && TRANSMITTER == 1
uint16_t get_rtc_bytes(void);
//void error_comunication(void);	// funkcja wykonujaca operacje na wypadek bledu komunikacji z kodowaniem
void send_protected_radio_bytes(uint16_t data);
#endif

#if DATA_SECURITY == 1 && RECEIVER == 1
void send_rtc_bytes(void);
uint8_t decode_frame(uint8_t f_byte, uint8_t s_byte);
uint8_t get_protected_radio_bytes(void);
#endif
/*
#if TRANSMITTER == 1 && DATA_SECURITY == 2
void send_radio_frame(uint8_t *data_bytes, uint8_t size);
#endif

#if RECEIVER == 1 && DATA_SECURITY == 2
void get_radio_frame(uint8_t *data_frame, uint8_t *RADIO_FLAG);
#endif
*/
#endif /* HC12_HC12_H_ */
