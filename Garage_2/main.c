/*
 * main.c
 *
 *  Created on: 1 maj 2021
 *      Author: Dominik
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "commands.h"
#include "RADIO_COMMUNICATION/radio.h"
#include "HC12/hc12.h"
#include "SENSORS/sensors.h"
#include "USART/usartlib.h"
//#include "LCD/lcd44780.h"


volatile uint16_t cnt_1 = 0, cnt_2 = 0, cnt_3 = 0;
volatile uint8_t state_sensors_2 = 0;
volatile uint16_t ms10_cnt = 0;
volatile uint8_t s1_cnt = 0;
volatile uint8_t measure_flag = 0x07;
volatile uint8_t radio_buff[10];
volatile uint8_t radio_flag = 0;


uint16_t temp_buff[5];
uint8_t temp_cnt = 0;
uint16_t distance = 0;
int8_t temperature = 0;
uint8_t ms500 = 0;
uint8_t ls1 = 0;

int main(void)
{
	usart_init(9600);
	//lcd_init();
	sensors_init();
	// ustawienie ADC
	ADMUX |= (1<<REFS0);
	ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1);

	UCSR0B |= (1<<RXCIE0);

	// **** P O D S T A W A    C Z A S U  --  10 ms **** //
	TCCR2A |= (1<<WGM21);			// tryb CTC
	TCCR2B |= (1<<CS22) | (1<<CS21) | (1<<CS20);	// preskaler: 1024
	OCR2A = 77;		// dodatkowy podzial przez 78
	TIMSK2 |= (1<<OCIE2A);

	// **** T I M E R    D O    C Z U J N I K O W    O D L E G L O S C I **** //
	TCCR0A |= (1<<WGM01);	// tryb CTC
	TCCR0B |= (1<<CS01);	// preskaler: 8
	OCR0A = 58;				// dodatkowy podzial przez 59
	TIMSK0 |= (1<<OCIE0A);

	// **** P R Z E R W A N I A    Z E W N E T R Z N E **** //
	PCICR |= (1<<PCIE2);
	PCMSK2 |= (1<<PCINT23) | (1<<PCINT22) | (1<<PCINT21) | (1<<PCINT20) | (1<<PCINT19);


	sei();
	while(1)
	{
		if(ms500 != (ms10_cnt / 50))	// wykonanie pomiarow dystansu i temperatury co 500 ms
		{
			// pomiar temperatury
			temp_buff[temp_cnt] = measure(0);
			temp_cnt++;
			if(temp_cnt > 4) temp_cnt = 0;
			uint16_t average = 0;
			for(uint8_t i = 0; i < 5; i++)
			{
				average += temp_buff[i];
			}
			average /= 5;
			average = 1023 - average;
			temperature = average / 5;	// regulacja dzielnika
			temperature -= 90;	// offset

			if(measure_flag & 0x07)
			{
				TRIG_1_ON;
				TRIG_2_ON;
				TRIG_3_ON;
				_delay_us(10);
				TRIG_1_OFF;
				TRIG_2_OFF;
				TRIG_3_OFF;
				TIMSK0 |= (1<<OCIE0A);	// zezwol na przerwania do mierzenia

				measure_flag &= ~0x07;
			}

			uint8_t data[2];
			data[0] = state_sensors_2;
			data[1] = temperature;
			send_radio_frame(data, 2, (MY_ADDRESS<<4) + GARAGE_1_ADDRESS, S2_DATA_2_S1);

			ms500 = (ms10_cnt / 50);
		}

		if(((measure_flag >> 3) & 0x07) == 0x07)
		{
			//lcd_cls();
			//uint16_t distance;
			cnt_1 -= 11;
			cnt_2 -= 44;
			cnt_3 -= 41;
			//distance = calculate_distance(cnt_1, cnt_2);
			if(cnt_1 > 8) state_sensors_2 |= 0x04;
			else state_sensors_2 &= ~0x04;

			if(cnt_2 > 8) state_sensors_2 |= 0x08;
			else state_sensors_2 &= ~0x08;

			if(cnt_3 > 8) state_sensors_2 |= 0x10;
			else state_sensors_2 &= ~0x10;

			//lcd_int(cnt_1);
			//lcd_str(" ");
			//lcd_int(cnt_2);
			//lcd_str(" ");
			//lcd_int(cnt_3);

			//lcd_int(cnt_1);
			//lcd_str(" ");
			//lcd_int(cnt_2);
			cnt_1 = 0;
			cnt_2 = 0;
			cnt_3 = 0;
			measure_flag &= ~(0x07 << 3); // wyzerowanie flagi zakonczenia pomiarow
			measure_flag |= 0x07;	// zezwolenie na wykonanie kolejnej serii pomiarow
		}

		if(radio_flag & 0x01)
		{
			execute_command((uint8_t *)radio_buff);
			radio_flag &= ~0x01;	// wyzeruj flage odebranych danych
		}
	}
}
ISR(USART0_RX_vect)
{
	get_radio_frame((uint8_t *)radio_buff, (uint8_t *)&radio_flag);
}
ISR(TIMER2_COMPA_vect)
{
	ms10_cnt++;
	if(ms10_cnt > 99) {s1_cnt++; ms10_cnt = 0;}
	if(s1_cnt > 59) s1_cnt = 0;
}
ISR(TIMER0_COMPA_vect)
{
	 if(!(MEAS_1_COMPLETED)) cnt_1++;		// licznik dlugosci impulsu na pinie ECHO
	 if(!(MEAS_2_COMPLETED)) cnt_2++;
	 if(!(MEAS_3_COMPLETED)) cnt_3++;
}
ISR(PCINT2_vect)
{
	static uint8_t previous_state = 0;
	uint8_t current_state;
	uint8_t change;

	current_state = PINC;
	current_state >>= 3;

	change = current_state ^ previous_state;

	if(change & 0x01)
	{
		if(IS_MOTION_1) state_sensors_2 |= (1<<0);
		else state_sensors_2 &= ~(1<<0);
	}
	if(change & 0x02)
	{
		if(IS_MOTION_2) state_sensors_2 |= (1<<1);
		else state_sensors_2 &= ~(1<<1);
	}
	if(change & 0x04)
	{
		if(!(current_state & (1<<2)))
		{
			// ustaw bit flagi na 1 gdy zakonczony pomiar
			measure_flag |= (1<<3);
			//lcd_int(cnt_1);
			// jesli wszystkie pomiary zakonczone to wylacz przerwania
			if((measure_flag & 0b111000) == 0b111000) TIMSK0 &= ~(1<<OCIE0A);
		}
	}
	if(change & 0x08)
	{
		if(!(current_state & (1<<3)))
		{
			// ustaw bit flagi na 1 gdy zakonczony pomiar
			measure_flag |= (1<<4);
			//lcd_int(cnt_2);
			// jesli wszystkie pomiary zakonczone to wylacz przerwania
			if((measure_flag & 0b111000) == 0b111000) TIMSK0 &= ~(1<<OCIE0A);
		}
	}
	if(change & 0x10)
	{
		if(!(current_state & (1<<4)))
		{
			// ustaw bit flagi na 1 gdy zakonczony pomiar
			measure_flag |= (1<<5);
			//lcd_int(cnt_3);
			// jesli wszystkie pomiary zakonczone to wylacz przerwania
			if((measure_flag & 0b111000) == 0b111000) TIMSK0 &= ~(1<<OCIE0A);
		}
	}
	//switch(change)
	//{
	//	case 1: if(IS_MOTION_1) state_sensors |= (1<<0);
	//			else state_sensors &= ~(1<<0);
	//			break;

	//	case 2: if(IS_MOTION_2) state_sensors |= (1<<1);
	//			else state_sensors &= ~(1<<1);
	//			break;
				// jesli pomiar zakonczony..
	//	case 4: if(!(current_state & (1<<2)))
	//				{
						// ustaw bit flagi na 1 gdy zakonczony pomiar
	//					measure_flag |= (1<<3);

						// jesli wszystkie pomiary zakonczone to wylacz przerwania
	//					if((measure_flag & 0b111000) == 0b111000) TIMSK0 &= ~(1<<OCIE0A);
	//				}
	//			break;
				// jesli pomiar zakonczony..
	//	case 8: if(!(current_state & (1<<3)))
	//				{
						// ustaw bit flagi na 1 gdy zakonczony pomiar
	//					measure_flag |= (1<<4);

						// jesli wszystkie pomiary zakonczone to wylacz przerwania
	//					if((measure_flag & 0b111000) == 0b111000) TIMSK0 &= ~(1<<OCIE0A);
	//				}
	//			break;
				// jesli pomiar zakonczony..
	//	case 16: if(!(current_state & (1<<4)))
	//				{
						// ustaw bit flagi na 1 gdy zakonczony pomiar
	//					measure_flag |= (1<<5);

						// jesli wszystkie pomiary zakonczone to wylacz przerwania
	//					if((measure_flag & 0b111000) == 0b111000) TIMSK0 &= ~(1<<OCIE0A);
	//				}
	//			break;
	//}
	previous_state = current_state;
}
