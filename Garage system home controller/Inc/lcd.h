/*
 * lcd.h
 *
 *  Created on: Dec 26, 2020
 *      Author: Dominik
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_



#include "stm32f1xx_hal.h"
#include "string.h"
#include "stdio.h"
#include "main.h"
//-----------------------------------------------------------------------------------------------------------
// *** Obsługa wyświetlaczy alfanumerycznych zgodnych z HD44780 ***
//
// - Sterowanie: tryb 4-bitowy
// - Dowolne przypisanie każdego sygnału sterującego do dowolnego pinu mikrokontrolera
// - Praca z pinem RW podłączonym do GND lub do mikrokontrolera (sprawdzanie BusyFLAG - szybkie operacje LCD)
//
// Pliki 			: lcd44780.c , lcd44780.h
// Mikrokontrolery 	: Atmel AVR
// Kompilator 		: avr-gcc
// Źródło 			: http://www.atnel.pl
// Data 			: marzec 2010
// Autor 			: Mirosław Kardaś
//----------------------------------------------------------------------------------------------------------
// Rozmiar kodu z załączonymi tylko funkcjami: lcd_init(), lcd_cls(), lcd_str() 				(RW<-->GND)
// dla procesorów AVR: 240 bajtów !!!
// Rozmiar kodu z załączonymi tylko funkcjami: lcd_init(), lcd_cls(), lcd_str(), lcd_locate()	(RW<-->GND)
// dla procesorów AVR: 254 bajty
// Rozmiar kodu z załączonymi tylko funkcjami: lcd_init(), lcd_cls(), lcd_str() 				(RW<-->uC)
// dla procesorów AVR: 326 bajtów !!!
// Rozmiar kodu z załączonymi tylko funkcjami: lcd_init(), lcd_cls(), lcd_str(), lcd_locate()	(RW<-->uC)
// dla procesorów AVR: 340 bajtów
//-----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//
//		Parametry pracy sterownika
//
//----------------------------------------------------------------------------------------
// rozdzielczość wyświetlacza LCD (wiersze/kolumny)
#define LCD_ROWS 4		// ilość wierszy wyświetlacza LCD
#define LCD_COLS 16	// ilość kolumn wyświetlacza LCD

// tu ustalamy za pomocą zera lub jedynki czy sterujemy pinem RW
//	0 - pin RW podłączony na stałe do GND
//	1 - pin RW podłączony do mikrokontrolera
#define USE_RW 0

//----------------------------------------------------------------------------------------
//
//		Ustawienia sprzętowe połączeń sterownika z mikrokontrolerem
//
//----------------------------------------------------------------------------------------
// tu konfigurujemy port i piny do jakich podłączymy linie D7..D4 LCD
#define LCD_D7PORT  A
#define LCD_D7 15
#define LCD_D6PORT  A
#define LCD_D6 12
#define LCD_D5PORT  A
#define LCD_D5 11
#define LCD_D4PORT  A
#define LCD_D4 10



// tu definiujemy piny procesora do których podłączamy sygnały RS,RW, E
#define LCD_RSPORT A
#define LCD_RS 9
#define LCD_RWPORT C
#define LCD_RW 11

#define LCD_EPORT A
#define LCD_E 8
//------------------------------------------------  koniec ustawień sprzętowych ---------------


//----------------------------------------------------------------------------------------
//****************************************************************************************
//*																						 *
//*		U S T A W I E N I A   KOMPILACJI												 *
//*																						 *
//*		Włączamy kompilację komend używanych lub wyłączamy nieużywanych					 *
//*		(dzięki temu regulujemy zajętość pamięci FLASH po kompilacji)					 *
//*																						 *
//*		1 - oznacza WŁĄCZENIE do kompilacji												 *
//*		0 - oznacza wyłączenie z kompilacji (funkcja niedostępna)						 *
//*																						 *
//****************************************************************************************
//----------------------------------------------------------------------------------------

#define USE_LCD_LOCATE	1			// ustawia kursor na wybranej pozycji Y,X (Y=0-3, X=0-n)

#define USE_LCD_CHAR 	1			// wysyła pojedynczy znak jako argument funkcji

#define USE_LCD_STR_P 	1			// wysyła string umieszczony w pamięci FLASH
#define USE_LCD_STR_E 	1			// wysyła string umieszczony w pamięci FLASH

#define USE_LCD_INT 	1			// wyświetla liczbę dziesietną na LCD
#define USE_LCD_HEX 	1			// wyświetla liczbę szesnastkową na LCD

#define USE_LCD_DEFCHAR		1		// wysyła zdefiniowany znak z pamięci RAM
#define USE_LCD_DEFCHAR_P 	1		// wysyła zdefiniowany znak z pamięci FLASH
#define USE_LCD_DEFCHAR_E 	1		// wysyła zdefiniowany znak z pamięci EEPROM

#define USE_LCD_CURSOR_ON 		0	// obsługa włączania/wyłączania kursora
#define USE_LCD_CURSOR_BLINK 	0	// obsługa włączania/wyłączania migania kursora
#define USE_LCD_CURSOR_HOME 	0	// ustawia kursor na pozycji początkowej

//------------------------------------------------  koniec ustawień kompilacji ---------------






// definicje adresów w DDRAM dla różnych wyświetlaczy
// inne są w wyświetlaczach 2wierszowych i w 4wierszowych
#if ( (LCD_ROWS == 4) && (LCD_COLS == 16) )
#define LCD_LINE1 0x00		// adres 1 znaku 1 wiersza
#define LCD_LINE2 0x28		// adres 1 znaku 2 wiersza
#define LCD_LINE3 0x14  	// adres 1 znaku 3 wiersza
#define LCD_LINE4 0x54  	// adres 1 znaku 4 wiersza
#else
#define LCD_LINE1 0x00		// adres 1 znaku 1 wiersza
#define LCD_LINE2 0x40		// adres 1 znaku 2 wiersza
#define LCD_LINE3 0x10  	// adres 1 znaku 3 wiersza
#define LCD_LINE4 0x50  	// adres 1 znaku 4 wiersza
#endif


// Makra upraszczające dostęp do portów
// *** PORT
#define GPIO(x) SPORT(x)
#define SPORT(x) (GPIO##x)
// *** PIN
//#define PIN(x) SPIN(x)
//#define SPIN(x) (PIN##x)
// *** DDR
//#define DDR(x) SDDR(x)
//#define SDDR(x) (DDR##x)


// Komendy sterujące
#define LCDC_CLS					0x01
#define LCDC_HOME					0x02
#define LCDC_ENTRY					0x04
	#define LCDC_ENTRYR					0x02
	#define LCDC_ENTRYL					0
	#define LCDC_MOVE					0x01
#define LCDC_ONOFF					0x08
	#define LCDC_DISPLAYON				0x04
	#define LCDC_CURSORON				0x02
	#define LCDC_CURSOROFF				0
	#define LCDC_BLINKON				0x01
#define LCDC_SHIFT					0x10
	#define LCDC_SHIFTDISP				0x08
	#define LCDC_SHIFTR					0x04
	#define LCDC_SHIFTL					0
#define LCDC_FUNC					0x20
	#define LCDC_FUNC8B					0x10
	#define LCDC_FUNC4B					0
	#define LCDC_FUNC2L					0x08
	#define LCDC_FUNC1L					0
	#define LCDC_FUNC5x10				0x04
	#define LCDC_FUNC5x7				0
#define LCDC_SET_CGRAM				0x40
#define LCDC_SET_DDRAM				0x80





// deklaracje funkcji na potrzeby innych modułów
void lcd_init(void);								// WŁĄCZONA na stałe do kompilacji
void lcd_cls(void);									// WŁĄCZONA na stałe do kompilacji
void lcd_str(char * str);							// WŁĄCZONA na stałe do kompilacji

void lcd_locate(uint8_t y, uint8_t x);				// domyślnie WŁĄCZONA z kompilacji w pliku lcd.c

void lcd_char(char c);								// domyślnie wyłączona z kompilacji w pliku lcd.c
void lcd_str_P(const char * str);							// domyślnie wyłączona z kompilacji w pliku lcd.c
void lcd_str_E(char * str);							// domyślnie wyłączona z kompilacji w pliku lcd.c
void lcd_int(int val);								// domyślnie wyłączona z kompilacji w pliku lcd.c
void lcd_hex(uint32_t val);								// domyślnie wyłączona z kompilacji w pliku lcd.c
void lcd_defchar(uint8_t nr, uint8_t *def_znak);	// domyślnie wyłączona z kompilacji w pliku lcd.c
void lcd_defchar_P(uint8_t nr, const uint8_t *def_znak);	// domyślnie wyłączona z kompilacji w pliku lcd.c
void lcd_defchar_E(uint8_t nr, uint8_t *def_znak);	// domyślnie wyłączona z kompilacji w pliku lcd.c

void lcd_home(void);								// domyślnie wyłączona z kompilacji w pliku lcd.c
void lcd_cursor_on(void);							// domyślnie wyłączona z kompilacji w pliku lcd.c
void lcd_cursor_off(void);							// domyślnie wyłączona z kompilacji w pliku lcd.c
void lcd_blink_on(void);							// domyślnie wyłączona z kompilacji w pliku lcd.c
void lcd_blink_off(void);							// domyślnie wyłączona z kompilacji w pliku lcd.c

#endif /* INC_LCD_H_ */
