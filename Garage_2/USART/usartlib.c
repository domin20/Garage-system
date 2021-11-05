/*
 * usartlib.c
 *
 *  Created on: 11 lut 2020
 *      Author: Dominik
 */

#include "usartlib.h"
#include <stdlib.h>
#include <avr/io.h>



/*
|
|
|
|
|
*/
void usart_init(unsigned int baud)
{
#if USART1_ON == 1
	uint16_t _ubr1 = (F_CPU/(16UL*UART_1_BAUD))-1; //dla U2Xn = 0
	/* ustawienie predkosci */
	UBRR1H = (uint8_t) (_ubr1>>8);
	UBRR1L = (uint8_t) _ubr1;
	/* Wlaczenie nadajnika i odbiornika */
	UCSR1B = (1<<RXEN1) | (1<<TXEN1);
	/* ustawienie formatu ramki: 8 bitów danych, 1 bit stopu */
	UCSR1C = (3<<UCSZ10);
#endif

	uint16_t _ubr = (F_CPU/(16UL*UART_BAUD))-1; //dla U2Xn = 0
	/* ustawienie predkosci */
	UBRR0H = (uint8_t) (_ubr>>8);
	UBRR0L = (uint8_t) _ubr;
	/* W³¹czenie nadajnika i odbiornika */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* ustawienie formatuu ramki: 8 bitów danych, 1 bit stopu */
	UCSR0C = (3<<UCSZ00);

}
void usart_char(char data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) )
		;
	/* Put data into buffer, sends the data */
	UDR0 = data;
}
void usart_int(int val)
{
  char buf[17];
  itoa(val,buf,10);
  usart_str(buf);
}
unsigned char usart_receive( void )
{
	/* oczekiwanie na dane */
	while ( !(UCSR0A & (1<<RXC0)) )
		;
	/* zwrot danych */
	return UDR0;
}
void usart_str(char *data )
{
		register char znak;

		while ((znak=*data++))  usart_char(znak);
}

//**************************** funkcje obsugujace transmisje dla USART1 *********************************
#if USART1_ON == 1

usart1_char(char data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE1)) )

	/* Put data into buffer, sends the data */
	UDR1 = data;
}
void usart1_int(int val)
{
  char buf[17];
  itoa(val,buf,10);
  usart1_str(buf);
}
unsigned char usart1_receive( void )
{
	/* oczekiwanie na dane */
	while ( !(UCSR1A & (1<<RXC1)) )
		;
	/* zwrot danych */
	return UDR1;
}
void usart1_str(char *data )
{
		register char znak;

		while ((znak=*data++))  usart1_char(znak);
}
#endif

