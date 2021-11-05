/*
 * ds3231.c
 *
 *  Created on: 25 lut 2020
 *      Author: Dominik
 */
#include "main.h"
#include "ds3231.h"
#include "lcd.h"

extern uint16_t cnt_for_animation;

extern I2C_HandleTypeDef hi2c1;

uint8_t dec2bcd(uint8_t dec)
{
	return ((dec/10)<<4) | (dec % 10);
}
uint8_t bcd2dec(uint8_t bcd)
{
	return ((((bcd)>>4) & 0x0F) * 10)+((bcd) & 0x0F);
}
uint8_t setyear(uint16_t year)
{
	uint8_t r;
	r=year-2000;
	return dec2bcd(r);
}
void set_time(uint8_t hh, uint8_t mm, uint8_t ss)
{
	uint8_t bufor[3];			//rezerwacja bufora 3 bajty
	bufor[0] = dec2bcd(ss);		//sekundy
	bufor[1] = dec2bcd(mm);		//minuty
	bufor[2] = dec2bcd(hh);		//godziny

	//zapis 3 bajt�w z bufora pod adresem 0x00 w pami�ci RAM naszego RTC
	//HAL_I2C_Mem_Write_IT(&hi2c1, 0x68<<1, 0x00, 1, bufor, 3);
	HAL_I2C_Mem_Write(&hi2c1, 0x68<<1, 0x00, 1, &bufor[0], 1, 50);
	HAL_I2C_Mem_Write(&hi2c1, 0x68<<1, 0x01, 1, &bufor[1], 1, 50);
	HAL_I2C_Mem_Write(&hi2c1, 0x68<<1, 0x02, 1, &bufor[2], 1, 50);
}
void set_date(uint8_t dayweek, uint8_t day, uint8_t month, uint8_t year)
{
	uint8_t bufor[4];
	bufor[0]=dec2bcd(dayweek);
	bufor[1]=dec2bcd(day);
	bufor[2]=dec2bcd(month);
	bufor[3]=dec2bcd(year);

	//HAL_I2C_Mem_Write_IT(&hi2c1, 0x68<<1, 0x03, 1, bufor, 4);
	HAL_I2C_Mem_Write(&hi2c1, 0x68<<1, 0x03, 1, &bufor[0], 1, 50);
	HAL_I2C_Mem_Write(&hi2c1, 0x68<<1, 0x04, 1, &bufor[1], 1, 50);
	HAL_I2C_Mem_Write(&hi2c1, 0x68<<1, 0x05, 1, &bufor[2], 1, 50);
	HAL_I2C_Mem_Write(&hi2c1, 0x68<<1, 0x06, 1, &bufor[3], 1, 50);
}
void show_day_of_week(uint8_t day)
{
	if(day==1) lcd_str("Pon");
	else if(day==2) lcd_str("Wto");
	else if(day==3) lcd_str("Sro");
	else if(day==4) lcd_str("Czw");
	else if(day==5) lcd_str("Pia");
	else if(day==6)	lcd_str("Sob");
	else if(day==7) lcd_str("Nie");
}
void show_month(uint8_t month)
{
	if(month == 1) lcd_str("Sty");
	else if(month == 2) lcd_str("Lut");
	else if(month == 3) lcd_str("Mar");
	else if(month == 4) lcd_str("Kwi");
	else if(month == 5) lcd_str("Maj");
	else if(month == 6) lcd_str("Cze");
	else if(month == 7) lcd_str("Lip");
	else if(month == 8) lcd_str("Sie");
	else if(month == 9) lcd_str("Wrz");
	else if(month == 10) lcd_str("Paz");
	else if(month == 11) lcd_str("Lis");
	else if(month == 12) lcd_str("Gru");
}

