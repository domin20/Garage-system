/*
 * radio.c
 *
 *  Created on: 27 kwi 2021
 *      Author: Dominik
 */
#include <avr/io.h>
#include "../USART/usartlib.h"
#include "radio.h"
#include "../HC12/hc12.h"
#include "../commands.h"
#include "../LCD/lcd44780.h"

extern volatile uint8_t radio_buff[15];
extern volatile uint8_t radio_flag;

void send_radio_frame(uint8_t *data_bytes, uint8_t size, uint8_t address, uint8_t command)
{
	if(size > MAX_DATA_BYTES) return; // jesli ilosc bajtow danych sie nie zgadza zakoncz dzialanie funkcji
	uint8_t frame[size+6];		// zmienna przechowujaca dane ramki do wyslania
	uint16_t check_bytes = 0;	// bajty sprawdzajace (dwa ostatnie w ramce)
	frame[0] = 255;				// pierwszy bajt = 255
	frame[1] = address;
	frame[2] = command;
	frame[3] = size;
	for(uint8_t i = 4; i < size + 4; i++)
	{
		frame[i] = *data_bytes++;		// zaladuj dane do ramki
		if(frame[i] == 255) frame[i] = 254;
		check_bytes += frame[i];		// dodaj wartosci danych do zmiennej sprawdzajace
	}
	for(uint8_t i = 0; i < 4; i++) check_bytes += frame[i];

	if((check_bytes & 0xff) == 255) check_bytes <<= 1; // jesli drugi bajt sprawdzajacy == 255 przesun bitowo o 1 w lewo
	frame[size+4] = check_bytes>>8;		//
	frame[size+5] = check_bytes;		// zaladuj bajty sprawdzajace do ramki

	for(uint8_t i = 0; i < size + 6; i++)
	{
		usart_char(frame[i]);			// wyslij cala ramke
	}
}

void get_radio_frame(void)
{
	static uint8_t cnt=0;		// zmienna licznikowa
	static uint8_t size, frame[13];		// size - zmienna przechowujaca ilosc bajtow danych/frame - zmienna przechowujaca ilosc bajtow w ramce
	uint8_t byte;			// byte - przechowuje bajt odebrany z uarta/ sum - przechowuje sume sprawdzajaca
	uint16_t sum=0, check_bytes=0;		// bajty sprawdzajace (dwa ostatnie w ramce)
	byte = usart_receive();		// odbieramy bajt z uarta
	if(byte == 255) {frame[0] = byte; cnt=1; return;}	// jesli rowny 255 -> pierwszy bajt ramki
	if(cnt == 1) {frame[1] = byte; cnt = 2; return;}	// adres urzadzen
	if((cnt == 2) && (byte <= NUM_OF_COMMANDS)) {frame[2] = byte; cnt = 3; return;}	// numer polecenia

	if((cnt == 3) && ((byte <= MAX_DATA_BYTES) && (byte >= 0)))	// jesli bajt rozmiaru ma poprawny format..
	{
		frame[3] = byte;			// zapisz do drugiego miejsca w ramce
		size = byte;				// zapisz do zmiennej size
		cnt = 4;					// zwieksz licznik
		return;						// zakoncz dzialanie funkcji
	}

	if((cnt >= 4) && (cnt < size + 6))	// odbieramy bajty danych..
	{
		frame[cnt] = byte;			// zapisujemy do miejsca w ramce
		cnt++;						// zwiekszamy licznik
		if(cnt == size + 6)			// jesli odebralismy juz wszystkie dane..
			{
				check_bytes = frame[cnt-2];		//
				check_bytes <<= 8;				//
				check_bytes |= frame[cnt-1];	//	odkodowywujemy check_bytes

				for(uint8_t i = 0; i < size + 4; i++)
				{
					sum += frame[i];	// sprawdzamy zgodnosc sumy bajtow danych i 1-go bajtu z check_bytes
				}
				if((sum & 0xff) == 255) sum <<= 1; // jesli drugi bajt sprawdzajacy = 255 przesun bitowo o 1 w lewo
				if(sum == check_bytes)	// jesli jest rowna..
				{
					cnt=0;			// zerujemy licznik
					for(uint8_t i = 0; i < size + 4; i++)
					{
						radio_buff[i] = frame[i];	// zapisujemy dane do bufora
					}
					radio_flag |= 0x01;	// ustawiamy flage odebranych i prawidlowych danych
				}
				else {cnt = 0; radio_flag &= ~ 0x01;}	// jesli nie ma zgodnosci sumy wyzeruj licznik i flage
			}
	}
}

