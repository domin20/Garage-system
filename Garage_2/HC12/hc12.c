/*
 * hc12.c
 *
 *  Created on: 17 wrz 2020
 *      Author: Dominik
 */
#include <avr/io.h>
#include <util/delay.h>
#include "hc12.h"
#include "../USART/usartlib.h"


uint8_t first_byte, second_byte;	// zmienne do ktorych zapisujemy bajty do wys³ania
uint8_t bufor[2];					// bufor przechowuj¹cy odebrane bajty kodujace
uint8_t r_byte_bufor[2];			// bufor dla sec i min do bajtow kodujacych(do wyslania)
uint8_t time[2];					// bufor potrzebny do pobrania danych z RTC (sekundy i minuty)
uint8_t seconds, minutes;			// zmienna przechowuj¹ca sekundy i minuty po zamianie bcd2dec

//**********************************************************************************************************
#if (TRANSMITTER == 1) || (RECEIVER == 1 && DATA_SECURITY == 1)
// funkcja wysylajaca bajt danych bez kodowania
void send_radio_byte(uint8_t byte)
{
	usart_char(byte);		// usart_int - dla terminala/ usart_char - dla mikrokontrolerow
}
#endif

//**********************************************************************************************************
#if RECEIVER == 1
// funkcja odbierajaca niezakodowany bajt
uint8_t get_radio_byte(void)
{
	uint8_t r_byte;
	r_byte = usart_receive();
	return r_byte;
}
#endif

//**********************************************************************************************************
#if DATA_SECURITY == 1 && TRANSMITTER == 1
// funckja odbierajaca bajty kodujace od modu³u odbiorczego
uint16_t get_rtc_bytes(void)
{
	uint16_t data;
	static uint8_t i=0;
	r_byte_bufor[i] = usart_receive();
	if(i == 0 && (r_byte_bufor[i]>>6 == 0)) {i++; return 0;} 	// bajt sekund - 7 bit zawsze = 0
	else if(i == 1 && (r_byte_bufor[i]>>6 == 1))				// bajt minut - 7 bit bit zawsze = 1
	{
		data = r_byte_bufor[0];
		data <<= 8;
		data |= r_byte_bufor[1];
		i=0;
		return data;
	}
	else {i=0; return 0;}
}
/*
void error_communication(void)
{
	// funkcja wykonujaca operacje na wypadek bledu komunikacji z kodowaniem
}
*/

// funkcja kodujaca i wysylajaca bajt danych
void send_protected_radio_bytes(uint16_t data)
{
	uint8_t sec, min;
	sec = data>>8;
	min = data;

	min &= 0b111111;

	// mo¿liwe wartoœci bajtu danych: 128-255
	// tworzenie bajtów kodujacych(pierwszego i drugiego) do wyslania

	//tworzenie 1 bajtu..
	first_byte = (sec & 0b11)<<5;
	first_byte |= (min & 0b10)<<3;
	first_byte |= (sec & 0b100)<<1;
	first_byte |= (min & 0b100);
	first_byte |= (sec & 0b10);
	first_byte |= (sec & 0b1000)>>3;

	//tworzenie 2 bajtu..
	second_byte = (sec & 0b110)<<4;
	second_byte |= (min & 0b11000);
	second_byte |= (sec & 0b11000)>>2;
	second_byte |= (sec & 0b100)>>2;

	// wys³anie dwoch bajtów..
	usart_char(first_byte);			// usart_int - dla terminala/ usart_char - dla mikrokontrolerow

	usart_char(second_byte);		// usart_int - dla terminala/ usart_char - dla mikrokontrolerow

}
#endif

//**********************************************************************************************************
#if DATA_SECURITY == 1 && RECEIVER == 1
// funkcja wysylajace bajty potrzebne do zakodowania
void send_rtc_bytes(void)
{
	i2c_init();
	I2C_read_buf(0x68<<1, 0x00, 2, time);	// pobranie sekund i minut, zapis do zmiennej
	seconds = bcd2dec(time[0]);
	minutes = bcd2dec(time[1]);

	minutes |= 0b1000000;

	usart_char(seconds);					// usart_int - dla terminala/ usart_char - dla mikrokontrolerow						// wyslanie sekund i minut do nadajnika
	usart_char(minutes);					// usart_int - dla terminala/ usart_char - dla mikrokontrolerow
}

// funkcja dekodujaca odebrana kompletna ramke
uint8_t decode_frame(uint8_t f_byte, uint8_t s_byte)
{
	uint8_t first_byte, second_byte;

	first_byte = (seconds & 0b11)<<5;
	first_byte |= (minutes & 0b10)<<3;
	first_byte |= (seconds & 0b100)<<1;
	first_byte |= (minutes & 0b100);
	first_byte |= (seconds & 0b10);
	first_byte |= (seconds & 0b1000)>>3;

	if(first_byte != f_byte) return FRAME_RESTART;	// jesli pierwszy bajt jest zly zwroc FRAME_RESTART

	second_byte = (seconds & 0b110)<<4;
	second_byte |= (minutes & 0b11000);
	second_byte |= (seconds & 0b11000)>>2;
	second_byte |= (seconds & 0b100)>>2;

	if(second_byte != s_byte) return FRAME_RESTART;	// jesli drugi bajt jest zly zwroc FRAME_RESTART
	else return FRAME_OK;			// jesli oba bajty sa poprawne zwroc FRAME_OK
}

// funkcja odbierajaca bajty danych lub bajty kodujace i wysyla bajty kodujace do funkcji dekodujacej
uint8_t get_protected_radio_bytes(void)
{
	static uint8_t cnt=0;			// zmienna licznikowa
	uint8_t x;
	bufor[cnt] = usart_receive();	// odbior nadeslanego bajtu i zapisanie do odpowiedniego bufora
	if(bufor[cnt]>127) {x = cnt; cnt = 0; return bufor[x];}	// jesli dolecial bajt danych -> odbierz i zwroc
	else if(cnt == 1) {
		if(decode_frame(bufor[0], bufor[1])) {cnt = 0; return 1;}	// jesli ramka ok to zwroc 1
		else {cnt = 0; return 0;}	// w przeciwnym wypadku cnt=0 i zwroc 0
	}
	else {cnt++; return 0;}
}
#endif
/*
#if TRANSMITTER == 1 && DATA_SECURITY == 2

void send_radio_frame(uint8_t *data_bytes, uint8_t size)
{
	if(size == 0 || size > MAX_DATA_BYTES) return; // jesli ilosc bajtow danych sie nie zgadza zakoncz dzialanie funkcji
	uint8_t frame[size+4];		// zmienna przechowujaca dane ramki do wyslania
	uint16_t check_bytes = 0;	// bajty sprawdzajace (dwa ostatnie w ramce)
	frame[0] = 255;				// pierwszy bajt = 255
	frame[1] = size;			// drugi bajt = rozmiar danych w ramce
	for(uint8_t i=2; i<size+2; i++)
	{
		if((*data_bytes)==255) (*data_bytes)=254;
		frame[i] = *data_bytes++;		// zaladuj dane do ramki
		check_bytes += frame[i];		// dodaj wartosci danych do zmiennej sprawdzajace
	}
	check_bytes += frame[0];
	if(check_bytes & 0xff == 255) check_bytes <<= 1; // jesli drugi bajt sprawdzajacy == 255 przesun bitowo o 1 w lewo

	frame[size+2] = check_bytes>>8;		//
	frame[size+3] = check_bytes;		// zaladuj bajty sprawdzajace do ramki

	for(uint8_t i=0; i<size+4; i++)		// zalecane jest uzycie przerwania od timera do wyslania
	{									// calej ramki (przerwanie co 5ms)
		usart_char(frame[i]);			// wyslij cala ramke
	}
}
#endif

#if RECEIVER == 1 && DATA_SECURITY == 2

void get_radio_frame(uint8_t *data_frame, uint8_t *RADIO_FLAG)
{
	static uint8_t cnt=0;		// zmienna licznikowa
	static uint8_t size, frame[MAX_DATA_BYTES+4];		// size - zmienna przechowujaca ilosc bajtow danych/frame - zmienna przechowujaca ilosc bajtow w ramce
	uint8_t byte;			// byte - przechowuje bajt odebrany z uarta/ sum - przechowuje sume sprawdzajaca
	uint16_t sum=0, check_bytes=0;		// bajty sprawdzajace (dwa ostatnie w ramce)
	byte = usart_receive();		// odbieramy bajt z uarta
	if(byte == 255) {frame[0] = byte; cnt=1; return;}	// jesli rowny 255 -> pierwszy bajt ramki

	if(cnt == 1 && (byte <= MAX_DATA_BYTES && byte > 0))	// jesli bajt rozmiaru ma poprawny format..
	{
		frame[cnt] = byte;			// zapisz do drugiego miejsca w ramce
		size = byte;				// zapisz do zmiennej size
		cnt = 2;					// zwieksz licznik
		return;						// zakoncz dzialanie funkcji
	}

	if((cnt >= 2) && (cnt < size+4))	// odbieramy bajty danych..
	{
		frame[cnt] = byte;			// zapisujemy do miejsca w ramce
		cnt++;						// zwiekszamy licznik
		if(cnt == size+4)			// jesli odebralismy juz wszystkie dane..
			{
				check_bytes = frame[cnt-2];		//
				check_bytes <<= 8;				//
				check_bytes |= frame[cnt-1];	//	odkodowywujemy check_bytes

				for(uint8_t i = 2; i < size+2; i++)
				{
					sum += frame[i];	// sprawdzamy zgodnosc sumy bajtow danych i 1-go bajtu z check_bytes
				}						//
				sum += frame[0];		//

				if(sum & 0xff == 255) sum <<= 1; // jesli drugi bajt sprawdzajacy = 255 przesun bitowo o 1 w lewo
				if(sum == check_bytes)	// jesli jest rowna..
				{
					cnt=0;			// zerujemy licznik
					for(uint8_t i=0; i<size; i++)
					{
						data_frame[i] = frame[i+2];	// zapisujemy dane do bufora
					}
					*RADIO_FLAG = 1;	// ustawiamy flage odebranych i prawidlowych danych
				}
				else {cnt = 0; *RADIO_FLAG = 0;}	// jesli nie ma zgodnosci sumy wyzeruj licznik i flage
			}
	}
}
#endif
*/
//**********************************************************************************************************
//***********   P R Z E R W A N I A    O D B I E R A J A C E    D A N E    Z    U A R T A    ***************
//**********   P R Z Y    P R Z E S Y L A N I U    D A N Y C H    Z    K O D O W A N I E M    **************
//**********************************************************************************************************
// dla nadajnika..
/*
ISR(USART0_RX_vect)
{
	uint16_t data;
	data = get_coding_bytes();		// zapisz bajty (sekundy i minuty) do zmiennej 16-bitowej
	if(data) send_protected_radio_bytes(data);	// jesli format poprawny wyslij zakodowane bajty
}

// dla odbiornika..
ISR(USART0_RX_vect)
{
	uint8_t byte;
	byte = get_protected_radio_bytes();
	if(byte>127) {bajt = byte; send_coding_bytes();} // "bajt" wlasna zmienna globalna przechowujaca bajt danych
	else if(byte == 1) status_frame = FRAME_OK;		// zalacz zmienna status_frame do main.c!
}
*/
