/*
 * usartlib.h
 *
 *  Created on: 11 lut 2020
 *      Author: Dominik
 */

#ifndef USARTLIB_H_
#define USARTLIB_H_

// 0 - USART1 wylaczony
// 1 - USART1 wlaczony
#define USART1_ON 0

#define UART_1_BAUD 9600	// tu defniujemy interesuj¹c¹ nas prêdkoœc dla usart1
#define UART_BAUD 9600 		// tu defniujemy interesuj¹c¹ nas prêdkoœc dla usart0
//#define _UBRR  (F_CPU/(8UL*UAT_BAUD))-1
void usart_init(unsigned int baud);		//inicjalizacja usart
void usart_char(char data);			//wyslanie znaku
void usart_int(int val);			//wyslanie liczby
void usart_str(char *data );		//wyslanie ciagu znakow
unsigned char usart_receive(void);	//odbior danych

#if USART1_ON == 1
void usart1_char(char data);		//wyslanie znaku
void usart1_int(int val);			//wyslanie liczby
void usart1_str(char *data );		//wyslanie ciagu znakow
unsigned char usart1_receive(void);	//odbior danych
#endif
#endif /* USARTLIB_H_ */
