/*
 * main.c
 *
 *  Created on: 28 kwi 2021
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


volatile uint16_t cnt_1 = 0, cnt_2 = 0;
volatile uint8_t state_sensors_1 = 0, state_sensors_2 = 0;
volatile uint8_t ms10_cnt = 0;
volatile uint8_t s1_cnt = 0;
volatile uint8_t measure_flag = 0x03;
volatile uint8_t radio_flag = 0;
volatile uint8_t radio_buff[15];

uint16_t temp_buff[5];
uint8_t temp_cnt = 0;
uint16_t distance = 0;
uint8_t dist_meas = 0;
int8_t temperature = 0, temperature_2 = 0;
uint8_t ls1 = 0;
uint8_t ms500 = 0;

int main(void)
{
	usart_init(9600);
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
	PCICR |= (1<<PCIE3);
	PCMSK3 |= (1<<PCINT29) | (1<<PCINT30) | (1<<PCINT31);


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

			uint8_t data[4];
			data[0] = state_sensors_1;
			data[1] = state_sensors_2;
			data[2] = temperature;
			data[3] = temperature_2;
			send_radio_frame(data, 4, (MY_ADDRESS<<4) + MASTER, S1_S2_DATA_2_MASTER);


			// pomiar czujnikow odleglosci
			if(measure_flag & 0x03)
			{
				TRIG_1_ON;
				TRIG_2_ON;
				_delay_us(10);
				TRIG_1_OFF;
				TRIG_2_OFF;
				TIMSK0 |= (1<<OCIE0A);	// zezwol na przerwania do mierzenia

				measure_flag &= ~0x03; // pomiar rozpoczety
			}
			ms500 = (ms10_cnt / 50);
		}

		// jesli pomiary czujnikow odleglosci zakonczone to oblicz
		if(((measure_flag >> 2) & 0x03) == 0x03)
		{
			//lcd_cls();
			uint16_t distance;
			cnt_1 -= 39;
			cnt_2 -= 39;
			distance = calculate_distance(cnt_1, cnt_2);
			if((distance >> 8) < 50) state_sensors_1 |= 0x04;
			else state_sensors_1 &= ~0x04;

			if((distance & 0xFF) < 50) state_sensors_1 |= 0x02;
			else state_sensors_1 &= ~0x02;

			//lcd_int(cnt_1);
			//lcd_str(" ");
			//lcd_int(cnt_2);
			cnt_1 = 0;
			cnt_2 = 0;
			measure_flag &= ~(0x03 << 2); // wyzerowanie flagi zakonczenia pomiarow
			measure_flag |= 0x03;	// zezwolenie na wykonanie kolejnej serii pomiarow
		}

		// obsluga nadciagajacych danych
		if(radio_flag & 0x01)
		{
			execute_command((uint8_t *)radio_buff);
			radio_flag &= ~ 0x01;	// wyzeruj flage odebranych danych
		}
	}
}
ISR(USART0_RX_vect)
{
	get_radio_frame();
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
}
ISR(PCINT3_vect)
{
	static uint8_t previous_state = 0;
	uint8_t current_state;
	uint8_t change;

	current_state = PIND;
	current_state >>= 5;

	change = current_state ^ previous_state;

	if(change & 0x01)
	{
		if(IS_MOTION_1) state_sensors_1 |= (1<<0);
		else state_sensors_1 &= ~(1<<0);
	}
	if(change & 0x02)
	{
		// jesli pomiar zakonczony..
		if(!(current_state & (1<<1)))
		{
			// ustaw bit flagi na 1 gdy zakonczony pomiar
			measure_flag |= (1<<2);

			// jesli wszystkie pomiary zakonczone to wylacz przerwania
			if((measure_flag & 0b1100) == 0b1100) TIMSK0 &= ~(1<<OCIE0A);
		}
	}
	if(change & 0x04)
	{
		if(!(current_state & (1<<2)))
		{
			// ustaw bit flagi na 1 gdy zakonczony pomiar
			measure_flag |= (1<<3);

			// jesli wszystkie pomiary zakonczone to wylacz przerwania
			if((measure_flag & 0b1100) == 0b1100) TIMSK0 &= ~(1<<OCIE0A);
		}
	}
	//switch(change)
	//{
	//	case 1: if(IS_MOTION_1) state_sensors_1 |= (1<<0);
	//			else state_sensors_1 &= ~(1<<0);
	//			break;

	//	case 2: // jesli pomiar zakonczony..
	//			if(!(current_state & (1<<1)))
	//			{
					// ustaw bit flagi na 1 gdy zakonczony pomiar
	//				measure_flag |= (1<<2);

					// jesli wszystkie pomiary zakonczone to wylacz przerwania
	//				if((measure_flag & 0b1100) == 0b1100) TIMSK0 &= ~(1<<OCIE0A);
	//			}
	//			break;
				// jesli pomiar zakonczony..
	//	case 4: if(!(current_state & (1<<2)))
	//				{
						// ustaw bit flagi na 1 gdy zakonczony pomiar
	//					measure_flag |= (1<<3);

						// jesli wszystkie pomiary zakonczone to wylacz przerwania
	//					if((measure_flag & 0b1100) == 0b1100) TIMSK0 &= ~(1<<OCIE0A);
	//				}
	//			break;
	//}
	previous_state = current_state;
}
