/*
 * ds3231.h
 *
 *  Created on: 25 lut 2020
 *      Author: Dominik
 */

#ifndef DS3231_DS3231_H_
#define DS3231_DS3231_H_

uint8_t dec2bcd(uint8_t dec);
uint8_t bcd2dec(uint8_t bcd);
void set_time(uint8_t hh, uint8_t mm, uint8_t ss);
void set_date(uint8_t dayweek, uint8_t day, uint8_t month, uint8_t year);
void show_day_of_week(uint8_t day);
void show_month(uint8_t month);

#endif /* DS3231_DS3231_H_ */
