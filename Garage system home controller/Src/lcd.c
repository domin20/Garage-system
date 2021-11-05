/*
 * lcd.c
 *
 *  Created on: Dec 26, 2020
 *      Author: Dominik
 */
#include "stm32f1xx_hal.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "main.h"


#include "lcd.h"
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


// makrodefinicje operacji na sygnałach sterujących RS,RW oraz E

#define SET_RS 	HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, GPIO_PIN_SET)			// stan wysoki na linii RS
#define CLR_RS 	HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, GPIO_PIN_RESET)		// stan niski na linii RS

#define SET_RW 	HAL_GPIO_WritePin(RW_GPIO_Port, RW_Pin, GPIO_PIN_SET)			// stan wysoki na RW - odczyt z LCD
#define CLR_RW 	HAL_GPIO_WritePin(RW_GPIO_Port, RW_Pin, GPIO_PIN_RESET)		// stan niski na RW - zapis do LCD

#define SET_E 	HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_SET)			// stan wysoki na linii E
#define CLR_E 	HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_RESET)			// stan niski na linii E


uint8_t check_BF(void);			// deklaracja funkcji wewnętrznej


//********************* FUNKCJE WEWNĘTRZNE *********************

//----------------------------------------------------------------------------------------
//
//		 Ustawienie wszystkich 4 linii danych jako WYjścia
//
//----------------------------------------------------------------------------------------
static inline void data_dir_out(void)
{
	//D7 PIN...
	if(LCD_D7 < 8) {
		GPIO(LCD_D7PORT)->CRL &= ~(0xF << LCD_D7*4);
		GPIO(LCD_D7PORT)->CRL |= (0x2 << LCD_D7*4);
	}
	else {
		GPIO(LCD_D7PORT)->CRH &= ~(0xF << LCD_D7*4);
		GPIO(LCD_D7PORT)->CRH |= (0x2 << LCD_D7*4);
	}

	// D6 PIN...
	if(LCD_D6 < 8){
		GPIO(LCD_D6PORT)->CRL &= ~(0xF << LCD_D6*4);
		GPIO(LCD_D6PORT)->CRL |= (0x2 << LCD_D6*4);
	}
	else {
		GPIO(LCD_D6PORT)->CRH &= ~(0xF << LCD_D6*4);
		GPIO(LCD_D6PORT)->CRH |= (0x2 << LCD_D6*4);
	}

	// D5 PIN...
	if(LCD_D5 < 8){
		GPIO(LCD_D5PORT)->CRL &= ~(0xF << LCD_D5*4);
		GPIO(LCD_D5PORT)->CRL |= (0x2 << LCD_D5*4);
	}
	else {
		GPIO(LCD_D5PORT)->CRH &= ~(0xF << LCD_D5*4);
		GPIO(LCD_D5PORT)->CRH |= (0x2 << LCD_D5*4);
	}

	// D4 PIN...
	if(LCD_D4 < 8){
		GPIO(LCD_D4PORT)->CRL &= ~(0xF << LCD_D4*4);
		GPIO(LCD_D4PORT)->CRL |= (0x2 << LCD_D4*4);
	}
	else {
		GPIO(LCD_D4PORT)->CRH &= ~(0xF << LCD_D4*4);
		GPIO(LCD_D4PORT)->CRH |= (0x2 << LCD_D4*4);
	}
	//GPIO(LCD_D7PORT)->CRL &= 0xFFFF0000;
	//GPIO(LCD_D7PORT)->CRL |= 0x2222;
}

//----------------------------------------------------------------------------------------
//
//		 Ustawienie wszystkich 4 linii danych jako WEjścia
//
//----------------------------------------------------------------------------------------
static inline void data_dir_in(void)
{
	//D7 PIN...
	if(LCD_D7 < 8) {
		GPIO(LCD_D7PORT)->CRL &= ~(0xF << LCD_D7*4);
		GPIO(LCD_D7PORT)->CRL |= (0x4 << LCD_D7*4);
	}
	else {
		GPIO(LCD_D7PORT)->CRH &= ~(0xF << LCD_D7*4);
		GPIO(LCD_D7PORT)->CRH |= (0x4 << LCD_D7*4);
	}

	// D6 PIN...
	if(LCD_D6 < 8) {
		GPIO(LCD_D6PORT)->CRL &= ~(0xF << LCD_D6*4);
		GPIO(LCD_D6PORT)->CRL |= (0x4 << LCD_D6*4);
	}
	else {
		GPIO(LCD_D6PORT)->CRH &= ~(0xF << LCD_D6*4);
		GPIO(LCD_D6PORT)->CRH |= (0x4 << LCD_D6*4);
	}

	// D5 PIN...
	if(LCD_D5 < 8) {
		GPIO(LCD_D5PORT)->CRL &= ~(0xF << LCD_D5*4);
		GPIO(LCD_D5PORT)->CRL |= (0x4 << LCD_D5*4);
	}
	else {
		GPIO(LCD_D5PORT)->CRH &= ~(0xF << LCD_D5*4);
		GPIO(LCD_D5PORT)->CRH |= (0x4 << LCD_D5*4);
	}

	// D4 PIN...
	if(LCD_D4 < 8) {
		GPIO(LCD_D4PORT)->CRL &= ~(0xF << LCD_D4*4);
		GPIO(LCD_D4PORT)->CRL |= (0x4 << LCD_D4*4);
	}
	else {
		GPIO(LCD_D4PORT)->CRH &= ~(0xF << LCD_D4*4);
		GPIO(LCD_D4PORT)->CRH |= (0x4 << LCD_D4*4);
	}
	//GPIO(LCD_D7PORT)->CRL &= 0xFFFF0000;
	//GPIO(LCD_D7PORT)->CRL |= 0x4444;
}

//----------------------------------------------------------------------------------------
//
//		 Wysłanie połówki bajtu do LCD (D4..D7)
//
//----------------------------------------------------------------------------------------
static inline void lcd_sendHalf(uint8_t data)
{
	if (data&(1<<0)) HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, GPIO_PIN_SET); else HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, GPIO_PIN_RESET);
	if (data&(1<<1)) HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, GPIO_PIN_SET); else HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, GPIO_PIN_RESET);
	if (data&(1<<2)) HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, GPIO_PIN_SET); else HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, GPIO_PIN_RESET);
	if (data&(1<<3)) HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_SET); else HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_RESET);
}

#if USE_RW == 1
//----------------------------------------------------------------------------------------
//
//		 Odczyt połówki bajtu z LCD (D4..D7)
//
//----------------------------------------------------------------------------------------
static inline uint8_t lcd_readHalf(void)
{
	uint8_t result=0;

	if(HAL_GPIO_ReadPin(D4_GPIO_Port, D4_Pin)) result |= (1<<0);
	if(HAL_GPIO_ReadPin(D5_GPIO_Port, D5_Pin)) result |= (1<<1);
	if(HAL_GPIO_ReadPin(D6_GPIO_Port, D6_Pin)) result |= (1<<2);
	if(HAL_GPIO_ReadPin(D7_GPIO_Port, D7_Pin)) result |= (1<<3);

	return result;
}
#endif

//----------------------------------------------------------------------------------------
//
//		 Zapis bajtu do wyświetlacza LCD
//
//----------------------------------------------------------------------------------------
void _lcd_write_byte(unsigned char _data)
{
	// Ustawienie pinów portu LCD D4..D7 jako wyjścia
	data_dir_out();

#if USE_RW == 1
	CLR_RW;
#endif

	SET_E;
	lcd_sendHalf(_data >> 4);			// wysłanie starszej części bajtu danych D7..D4
	CLR_E;

	SET_E;
	lcd_sendHalf(_data);				// wysłanie młodszej części bajtu danych D3..D0
	CLR_E;

#if USE_RW == 1
	while( (check_BF() & (1<<7)) );
#else
	HAL_Delay(1); // powinno byc 120 us, a nie 120 ms
#endif

}

#if USE_RW == 1
//----------------------------------------------------------------------------------------
//
//		 Odczyt bajtu z wyświetlacza LCD
//
//----------------------------------------------------------------------------------------
uint8_t _lcd_read_byte(void)
{
	uint8_t result=0;
	data_dir_in();

	SET_RW;

	SET_E;
	result = (lcd_readHalf() << 4);	// odczyt starszej części bajtu z LCD D7..D4
	CLR_E;

	SET_E;
	result |= lcd_readHalf();			// odczyt młodszej części bajtu z LCD D3..D0
	CLR_E;

	return result;
}
#endif


#if USE_RW == 1
//----------------------------------------------------------------------------------------
//
//		 Sprawdzenie stanu Busy Flag (Zajętości wyświetlacza)
//
//----------------------------------------------------------------------------------------
uint8_t check_BF(void)
{
	CLR_RS;
	return _lcd_read_byte();
}
#endif


//----------------------------------------------------------------------------------------
//
//		 Zapis komendy do wyświetlacza LCD
//
//----------------------------------------------------------------------------------------
void lcd_write_cmd(uint8_t cmd)
{
	CLR_RS;
	_lcd_write_byte(cmd);
}

//----------------------------------------------------------------------------------------
//
//		 Zapis danych do wyświetlacza LCD
//
//----------------------------------------------------------------------------------------
void lcd_write_data(uint8_t data)
{
	SET_RS;
	_lcd_write_byte(data);
}




//**************************  FUNKCJE PRZEZNACZONE TAKŻE DLA INNYCH MODUŁÓW  ******************

#if USE_LCD_CHAR == 1
//----------------------------------------------------------------------------------------
//
//		 Wysłanie pojedynczego znaku do wyświetlacza LCD w postaci argumentu
//
//		 8 własnych znaków zdefiniowanych w CGRAM
//		 wysyłamy za pomocą kodów 0x80 do 0x87 zamiast 0x00 do 0x07
//
//----------------------------------------------------------------------------------------
void lcd_char(char c)
{
	lcd_write_data( ( c>=0x80 && c<=0x87 ) ? (c & 0x07) : c);
}
#endif

//----------------------------------------------------------------------------------------
//
//		 Wysłanie stringa do wyświetlacza LCD z pamięci RAM
//
//
//----------------------------------------------------------------------------------------
void lcd_str(char * str)
{
	register char znak;
	while ( (znak=*(str++)) ) lcd_char( znak );
}

#if USE_LCD_STR_P == 1
//----------------------------------------------------------------------------------------
//
//		 Wysłanie stringa do wyświetlacza LCD z pamięci FLASH
//
//----------------------------------------------------------------------------------------

#endif


#if USE_LCD_STR_E == 1
//----------------------------------------------------------------------------------------
//
//		 Wysłanie stringa do wyświetlacza LCD z pamięci EEPROM
//
//		 8 własnych znaków zdefiniowanych w CGRAM
//		 wysyłamy za pomocą kodów 0x80 do 0x87 zamiast 0x00 do 0x07
//
//----------------------------------------------------------------------------------------

#endif
#if USE_LCD_INT == 1
//----------------------------------------------------------------------------------------
//
//		 Wyświetla liczbę dziesiętną na wyświetlaczu LCD
//
//----------------------------------------------------------------------------------------
void lcd_int(int val)
{
	char bufor[17];
	lcd_str( itoa(val, bufor, 10) );
}
#endif

#if USE_LCD_HEX == 1
//----------------------------------------------------------------------------------------
//
//		 Wyświetla liczbę szestnastkową HEX na wyświetlaczu LCD
//
//----------------------------------------------------------------------------------------
void lcd_hex(uint32_t val)
{
	char bufor[17];
	lcd_str( itoa(val, bufor, 16) );
}
#endif

#if USE_LCD_DEFCHAR == 1
//----------------------------------------------------------------------------------------
//
//		Definicja własnego znaku na LCD z pamięci RAM
//
//		argumenty:
//		nr: 		- kod znaku w pamięci CGRAM od 0x80 do 0x87
//		*def_znak:	- wskaźnik do tablicy 7 bajtów definiujących znak
//
//----------------------------------------------------------------------------------------
void lcd_defchar(uint8_t nr, uint8_t *def_znak)
{
	register uint8_t i,c;
	lcd_write_cmd( 64+((nr&0x07)*8) );
	for(i=0;i<8;i++)
	{
		c = *(def_znak++);
		lcd_write_data(c);
	}
}
#endif

#if USE_LCD_DEFCHAR_P == 1
//----------------------------------------------------------------------------------------
//
//		Definicja własnego znaku na LCD z pamięci FLASH
//
//		argumenty:
//		nr: 		- kod znaku w pamięci CGRAM od 0x80 do 0x87
//		*def_znak:	- wskaźnik do tablicy 7 bajtów definiujących znak
//
//----------------------------------------------------------------------------------------

#endif

#if USE_LCD_DEFCHAR_E == 1
//----------------------------------------------------------------------------------------
//
//		Definicja własnego znaku na LCD z pamięci EEPROM
//
//		argumenty:
//		nr: 		- kod znaku w pamięci CGRAM od 0x80 do 0x87
//		*def_znak:	- wskaźnik do tablicy 7 bajtów definiujących znak
//
//----------------------------------------------------------------------------------------
/*
void lcd_defchar_E(uint8_t nr, uint8_t *def_znak)
{
	register uint8_t i,c;

	lcd_write_cmd( 64+((nr&0x07)*8) );
	for(i=0;i<8;i++)
	{
		c = eeprom_read_byte(def_znak++);
		lcd_write_data(c);
	}
}
*/
#endif


#if USE_LCD_LOCATE == 1
//----------------------------------------------------------------------------------------
//
//		Ustawienie kursora w pozycji Y-wiersz, X-kolumna
//
// 		Y = od 0 do 3
// 		X = od 0 do n
//
//		funkcja dostosowuje automatycznie adresy DDRAM
//		w zależności od rodzaju wyświetlacza (ile posiada wierszy)
//
//----------------------------------------------------------------------------------------
void lcd_locate(uint8_t y, uint8_t x)
{
	switch(y)
	{
		case 0: y = LCD_LINE1; break;

#if (LCD_ROWS>1)
	    case 1: y = LCD_LINE2; break; // adres 1 znaku 2 wiersza
#endif
#if (LCD_ROWS>2)
    	case 2: y = LCD_LINE3; break; // adres 1 znaku 3 wiersza
#endif
#if (LCD_ROWS>3)
    	case 3: y = LCD_LINE4; break; // adres 1 znaku 4 wiersza
#endif
	}

	lcd_write_cmd( (0x80 + y + x) );
}
#endif


//----------------------------------------------------------------------------------------
//
//		Kasowanie ekranu wyświetlacza
//
//----------------------------------------------------------------------------------------
void lcd_cls(void)
{
	lcd_write_cmd( LCDC_CLS );

	#if USE_RW == 0
		HAL_Delay(5);
	#endif
}


#if USE_LCD_CURSOR_HOME == 1
//----------------------------------------------------------------------------------------
//
//		Powrót kursora na początek
//
//----------------------------------------------------------------------------------------
void lcd_home(void)
{
	lcd_write_cmd( LCDC_CLS|LCDC_HOME );

	#if USE_RW == 0
		HAL_Delay(5);
	#endif
}
#endif

#if USE_LCD_CURSOR_ON == 1
//----------------------------------------------------------------------------------------
//
//		Włączenie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_cursor_on(void)
{
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON|LCDC_CURSORON);
}

//----------------------------------------------------------------------------------------
//
//		Wyłączenie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_cursor_off(void)
{
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON);
}
#endif


#if USE_LCD_CURSOR_BLINK == 1
//----------------------------------------------------------------------------------------
//
//		WŁącza miganie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_blink_on(void)
{
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON|LCDC_CURSORON|LCDC_BLINKON);
}

//----------------------------------------------------------------------------------------
//
//		WYłącza miganie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_blink_off(void)
{
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON);
}
#endif




//----------------------------------------------------------------------------------------
//
//		 ******* INICJALIZACJA WYŚWIETLACZA LCD ********
//
//----------------------------------------------------------------------------------------
void lcd_init(void)
{
	// inicjowanie pinów portów ustalonych do podłączenia z wyświetlaczem LCD
	// ustawienie wszystkich jako wyjścia
	data_dir_out();
	GPIO(LCD_RSPORT)->CRH &= 0xFFF2F2FF;
	#if USE_RW == 1
		GPIO(LCD_RWPORT)->CRH &= 0xFFFF2FFF;
	#endif

	HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_SET);
	#if USE_RW == 1
		HAL_GPIO_WritePin(RW_GPIO_Port, RW_Pin, GPIO_PIN_SET);
	#endif

	HAL_Delay(15);
	HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, GPIO_PIN_RESET);
	#if USE_RW == 1
		HAL_GPIO_WritePin(RW_GPIO_Port, RW_Pin, GPIO_PIN_RESET);
	#endif

	// jeszcze nie można używać Busy Flag
	SET_E;
	lcd_sendHalf(0x03);	// tryb 8-bitowy
	CLR_E;
	HAL_Delay(5);

	SET_E;
	lcd_sendHalf(0x03);	// tryb 8-bitowy
	CLR_E;
	HAL_Delay(1);	// powinno byc 100 us!!!!!!!!!!!!!!!!!!!!!!

	SET_E;
	lcd_sendHalf(0x03);	// tryb 8-bitowy
	CLR_E;
	HAL_Delay(1);	// powinno byc 100 us !!!!!!!!!!!!!!!!!!!!!!!

	SET_E;
	lcd_sendHalf(0x02);// tryb 4-bitowy
	CLR_E;
	HAL_Delay(1);		// powinno byc 100 us !!!!!!!!!!!!!!!!!!

	// już można używać Busy Flag
	// tryb 4-bitowy, 2 wiersze, znak 5x7
	lcd_write_cmd( LCDC_FUNC|LCDC_FUNC4B|LCDC_FUNC2L|LCDC_FUNC5x7 );
	// wyłączenie kursora
	lcd_write_cmd( LCDC_ONOFF|LCDC_CURSOROFF );
	// włączenie wyświetlacza
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON );
	// przesuwanie kursora w prawo bez przesuwania zawartości ekranu
	lcd_write_cmd( LCDC_ENTRY|LCDC_ENTRYR );
/*
	static const uint8_t tab1[] PROGMEM = {0,0,14,1,15,17,15,2};		// litera "ą"
	static const uint8_t tab2[] PROGMEM = {4,4,4,6,12,4,31,0};			// litera "ł"
	static const uint8_t tab3[] PROGMEM = {16,16,18,20,24,16,31,0};		// litera "duże ł"
	static const uint8_t tab4[] PROGMEM = {2,4,31,2,4,8,31,0};			// litera "ź"
	static const uint8_t tab5[] PROGMEM = {0,0,14,17,31,16,14,4};		// litera "ę"
	static const uint8_t tab6[] PROGMEM = {2,4,14,16,14,1,30,0};		// litera "ś"
	static const uint8_t tab7[] PROGMEM = {0,4,31,2,4,8,31,0};			// litera "ż"
	static const uint8_t tab8[] PROGMEM = {2,4,14,17,17,17,14,0};		// litera "ó"

	lcd_defchar_P(0x80, tab1);		// "ą"
	lcd_defchar_P(0x81, tab2);		// "ł"
	lcd_defchar_P(0x82, tab3);		// duze "ł"
	lcd_defchar_P(0x83, tab4);		// "ź"
	lcd_defchar_P(0x84, tab5);		// "ę"
	lcd_defchar_P(0x85, tab6);		// "ś"
	lcd_defchar_P(0x86, tab7);		// "ż"
	lcd_defchar_P(0x87, tab8);		// "ó"
*/
	// kasowanie ekranu
	lcd_cls();
}

