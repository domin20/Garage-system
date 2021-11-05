/*
 * interface.c
 *
 *  Created on: May 16, 2021
 *      Author: Dominik
 */
#include "main.h"
#include "interface.h"
#include "ds3231.h"
#include "lcd.h"


extern uint8_t refresh_lcd;
extern uint8_t alarm_flag;
extern uint8_t state;
extern uint16_t cnt_for_animation;
extern uint8_t temperature_1, temperature_2, room_temperature;
extern uint8_t menu_cnt;
extern uint8_t alarm_1_buff[4][8];
extern uint8_t alarm_2_buff[4][8];
extern uint8_t alarm_menu_cnt;
extern uint8_t sec, min, hour, day, month, dayweek;

void execute_kn4x4_command(char c)
{
	switch(state)
	{
		  case 1: if(c == '#') {state = 2; refresh_lcd = 1;}
		  	  	  else if(c == '*'){
		  	  		  if(alarm_flag & 0x02)
		  	  		  {
		  	  			 alarm_flag &= ~0x02;
		  	  			 lcd_locate(0,0);
		  	  			 lcd_str("Alarm zostal        ");
		  	  			 lcd_locate(1,0);
		  	  			 lcd_str("uzbrojony           ");
		  	  			 lcd_locate(2,0);
		  	  			 display_empty_line;
		  	  			 cnt_for_animation = 2000;
		  	  			 refresh_lcd = 1;
		  	  		  }
		  	  		  else
		  	  		  {
		  	  			 alarm_flag |= 0x02;
		  	  			 lcd_locate(0,0);
		  	  			 lcd_str("Alarm zostal        ");
		  	  			 lcd_locate(1,0);
		  	  			 lcd_str("rozbrojony          ");
		  	  			 lcd_locate(2,0);
		  	  			 display_empty_line;
		  	  			 cnt_for_animation = 2000;
		  	  			 refresh_lcd = 1;
		  	  		  }
		  	  	  }
		  	  	  break;

		  case 2: if(c == '2')
		  	  	  {
			   	   	  if(menu_cnt > 1) {menu_cnt--; refresh_lcd = 1;}
		  	  	  }
		  	  	  else if(c == '8')
		  	  	  {
		  	  		  if(menu_cnt < 5) {menu_cnt++; refresh_lcd = 1;}
		  	  	  }
		  	  	  else if(c == 'B') {state = 1; refresh_lcd = 1;}
		  	  	  else if(c == 'A')
		  	  	  {
		  	  		  state = 3;
		  	  		  refresh_lcd = 1;
		  	  	  }
		  	  	  break;
		  case 3: if(menu_cnt == 1) create_new_time(c);
		  	  	  else if(menu_cnt == 2) create_new_date(c);
		  	  	  else if(menu_cnt == 3)
		  	  	  {
		  	  		  if(c == 'B'){
		  	  			  state = 2;
		  	  			  refresh_lcd = 1;
		  	  		  }
		  	  	  }
		  	  	  else if(menu_cnt == 4)
		  	  	  {
		  	  		  if(c == '2') {
		  	  			  if(alarm_menu_cnt > 1) {alarm_menu_cnt--; refresh_lcd = 1;}
		  	  		  }
		  	  		  else if(c == '8'){
		  	  			  if(alarm_menu_cnt < 8) {alarm_menu_cnt++; refresh_lcd = 1;}
		  	  		  }
		  	  		  else if(c == 'A'){
		  	  			  state = 4;
		  	  			  refresh_lcd = 1;
		  	  		  }
		  	  		  else if(c == 'B'){
		  	  			  state = 2;
		  	  			  refresh_lcd = 1;
		  	  		  }
		  	  	  }
		  	  	  break;
		  case 4: if(c == 'B'){
			  	  	  state = 3;
			  	  	  refresh_lcd = 1;
		  	  	  }
		  	  	  break;
	}
}
void create_new_time(char c)
{
	static uint8_t d_hours = 0, j_hours = 0;
	static uint8_t d_minutes = 0, j_minutes = 0;
	static uint8_t d_seconds = 0, j_seconds = 0;
	static uint8_t progress = 0;

	switch(progress)
	{
		case 0: if(c >= '0' && c <= '2') {
					d_hours = c - 48;
					lcd_locate(0,3);
					lcd_int(d_hours);
					progress++;
				}
				else if(c == 'B') {progress = 0; state = 1; refresh_lcd = 1;}
				break;
		case 1: if(c >= '0' && c <= '3') {
					j_hours = c - 48;
					lcd_int(j_hours);
					lcd_str("  ");
					lcd_locate(1,0);
					lcd_str("mm:  <-");
					progress++;
				}
				else if(c == 'B') {progress = 0; state = 1; refresh_lcd = 1;}
				break;
		case 2: if(c >= '0' && c <= '5') {
					d_minutes = c - 48;
					lcd_locate(1,0);
					lcd_str("mm:");
					lcd_int(d_minutes);
					progress++;
				}
				else if(c == 'B') {progress = 0; state = 1; refresh_lcd = 1;}
				break;
		case 3: if(c >= '0' && c <= '9') {
					j_minutes = c - 48;
					lcd_locate(1,0);
					lcd_str("mm:");
					lcd_int(d_minutes);
					lcd_int(j_minutes);
					lcd_str("  ");
					lcd_locate(2,5);
					lcd_str("<-");
					lcd_locate(2,3);
					progress++;
				}
				else if(c == 'B') {progress = 0; state = 1; refresh_lcd = 1;}
				break;
		case 4: if(c >= '0' && c <= '5') {
					d_seconds = c - 48;
					lcd_int(d_seconds);
					progress++;
				}
				else if(c == 'B') {progress = 0; state = 1; refresh_lcd = 1;}
				break;
		case 5: if(c >= '0' && c <= '9') {
					j_seconds = c - 48;
					lcd_int(j_seconds);
					progress++;
				}
				else if(c == 'B') {progress = 0; state = 1; refresh_lcd = 1;}
				break;
		case 6: if(c == 'A')
				{
					lcd_locate(3,0);
					lcd_str("Ust.czas? A-YES B-NO");
					progress++;
				}
				else if(c == 'B') {progress = 0; state = 1; refresh_lcd = 1;}
				break;
		case 7: if(c == 'A')
				{
					uint8_t h = 0, m = 0, s = 0;
					h = 10*d_hours + j_hours;
					m = 10*d_minutes + j_minutes;
					s = 10*d_seconds + j_seconds;

					set_time(h,m,s);
					progress = 0;
					state = 1;
					lcd_locate(3,0);
					lcd_str("Ustawiono nowy czas.");
					cnt_for_animation = 2000;
					refresh_lcd = 1;
				}
				else if(c == 'B') progress--;
				break;
	}
}
void create_new_date(char c)
{
	static uint8_t dayweek = 0;
	static uint8_t d_day = 0, j_day = 0;
	static uint8_t d_month = 0, j_month = 0;
	static uint8_t d_year = 0, j_year = 0;

	static uint8_t progress = 0;

	switch(progress)
	{
		case 0: if((c >= '0') && (c < '8')){
					dayweek = c - 48;
					lcd_locate(0,6);
					lcd_int(dayweek);
					lcd_str("  ");
					lcd_locate(1,0);
					lcd_str("dzien:  <-");
					progress++;
				}
				else if(c == 'B'){
					progress = 0;
					state = 1;
					refresh_lcd = 1;
				}
				break;
		case 1: if((c >= '0') && (c < '4'))
				{
					d_day = c - 48;
					lcd_locate(1,0);
					lcd_str("dzien:");
					lcd_int(d_day);
					progress++;
				}
				else if(c == 'B'){
					progress = 0;
					state = 1;
					refresh_lcd = 1;
				}
				break;
		case 2: if(((d_day == 3) && ((c >= '0') &&(c < '2'))) || (d_day < 3))
				{
					j_day = c - 48;
					lcd_locate(1,0);
					lcd_str("dzien:");
					lcd_int(d_day);
					lcd_int(j_day);
					lcd_str("  ");
					lcd_locate(2,7);
					lcd_str("<-");
					progress++;
				}
				else if(c == 'B'){
					progress = 0;
					state = 1;
					refresh_lcd = 1;
				}
				break;
		case 3: if((c >= '0') && (c < '2'))
				{
					d_month = c - 48;
					lcd_locate(2,5);
					lcd_int(d_month);
					progress++;
				}
				else if(c == 'B'){
					progress = 0;
					state = 1;
					refresh_lcd = 1;
				}
				break;
		case 4: if(((d_month == 1) && ((c >= '0') && (c < '3'))) || ((c >= '0') && (c <= '9')))
				{
					j_month = c - 48;
					lcd_locate(2,6);
					lcd_int(j_month);
					lcd_str("  ");
					lcd_locate(3,6);
					lcd_str("<-");
					progress++;
				}
				else if(c == 'B'){
					progress = 0;
					state = 1;
					refresh_lcd = 1;
				}
				break;
		case 5: if(c == 'B'){
					progress = 0;
					state = 1;
					refresh_lcd = 1;
				}
				else if((c >= '0') && (c <= '9')){
					d_year = c - 48;
					lcd_locate(3,4);
					lcd_int(d_year);
					progress++;
				}
				break;
		case 6: if(c == 'B'){
					progress = 0;
					state = 1;
					refresh_lcd = 1;
				}
				else if((c >= '0') && (c <= '9')){
					j_year = c - 48;
					lcd_locate(3,5);
					lcd_int(j_year);
					progress++;
				}
				break;
		case 7: if(c == 'A') {
					lcd_locate(0,0);
					lcd_str("Ustawic nowa date?  ");
					lcd_locate(1,0);
					lcd_str("A-YES   B-NO        ");
					lcd_locate(2,0);
					display_empty_line;
					lcd_locate(3,0);
					display_empty_line;
					progress++;
				}
				else if(c == 'B'){
					progress = 0;
					state = 1;
					refresh_lcd = 1;
				}
				break;
		case 8: if(c == 'A'){
					lcd_locate(0,0);
					lcd_str("Ustawiono nowa date.");
					lcd_locate(1,0);
					display_empty_line;
					uint8_t d = 0, m = 0, y = 0;

					d = 10*d_day + j_day;
					m = 10*d_month + j_month;
					y = 10*d_year + j_year;
					set_date(dayweek, d, m, y);

					cnt_for_animation = 2000;
					progress = 0;
					state = 1;
					refresh_lcd = 1;
				}
				else if(c == 'B') {
					state = 1;
					progress = 0;
					refresh_lcd = 1;
				}
				break;
	}
}
void show_saved_alarms(void)
{
	lcd_locate(0,0);

	if(alarm_menu_cnt < 5) {
		uint8_t idx = alarm_menu_cnt - 1;
		lcd_str("Zapisany alarm 1: ");
		lcd_int(alarm_menu_cnt);
		lcd_locate(1,0);
		show_day_of_week(alarm_1_buff[idx][0]);
		lcd_str(", ");
		if(alarm_1_buff[idx][1] < 10) lcd_int(0);
		lcd_int(alarm_1_buff[idx][1]);
		lcd_str(" ");
		show_month(alarm_1_buff[idx][2]);
		lcd_str(" ");
		lcd_int(2000 + alarm_1_buff[idx][3]);

		lcd_locate(2,0);
		lcd_str("Time: ");
		if(alarm_1_buff[idx][4] < 10) lcd_int(0);
		lcd_int(alarm_1_buff[idx][4]);
		lcd_str(":");
		if(alarm_1_buff[idx][5] < 10) lcd_int(0);
		lcd_int(alarm_1_buff[idx][5]);
		lcd_str(":");
		if(alarm_1_buff[idx][6] < 10) lcd_int(0);
		lcd_int(alarm_1_buff[idx][6]);

		lcd_locate(3,0);
		lcd_str("st.sen: ");
		lcd_int((alarm_1_buff[idx][7] & 0x04) >> 2);
		lcd_char('|');
		lcd_int((alarm_1_buff[idx][7] & 0x02) >> 1);
		lcd_char('|');
		lcd_int(alarm_1_buff[idx][7] & 0x01);
	}
	else {
		uint8_t idx = alarm_menu_cnt - 5;
		lcd_str("Zapisany alarm 2: ");
		lcd_int(alarm_menu_cnt - 4);

		lcd_locate(1,0);
		show_day_of_week(alarm_2_buff[idx][0]);
		lcd_str(", ");
		if(alarm_2_buff[idx][1] < 10) lcd_int(0);
		lcd_int(alarm_2_buff[idx][1]);
		lcd_str(" ");
		show_month(alarm_2_buff[idx][2]);
		lcd_str(" ");
		lcd_int(2000 + alarm_2_buff[idx][3]);

		lcd_locate(2,0);
		lcd_str("Time: ");
		if(alarm_2_buff[idx][4] < 10) lcd_int(0);
		lcd_int(alarm_2_buff[idx][4]);
		lcd_str(":");
		if(alarm_2_buff[idx][5] < 10) lcd_int(0);
		lcd_int(alarm_2_buff[idx][5]);
		lcd_str(":");
		if(alarm_2_buff[idx][6] < 10) lcd_int(0);
		lcd_int(alarm_2_buff[idx][6]);

		lcd_locate(3,0);
		lcd_str("st.sen: ");
		lcd_int((alarm_2_buff[idx][7] & 0x10) >> 4);
		lcd_char('|');
		lcd_int((alarm_2_buff[idx][7] & 0x08) >> 3);
		lcd_char('|');
		lcd_int((alarm_2_buff[idx][7] & 0x04) >> 2);
		lcd_char('|');
		lcd_int((alarm_2_buff[idx][7] & 0x02) >> 1);
		lcd_char('|');
		lcd_int(alarm_2_buff[idx][7] & 0x01);
	}

}
void handling_saved_alarms(void)
{

}
void refresh_display(void)
{
	if(cnt_for_animation == 0)
	{
		switch(state)
		{
		case 1:	lcd_locate(0,0);
				lcd_str("Tr=");
				lcd_int(room_temperature);
				lcd_str("   ");
				lcd_locate(0,6);
				lcd_str("              ");
				// tu Tr

				lcd_locate(1,0);
				lcd_str("T1=");
				lcd_int(temperature_1);
				lcd_str("              ");
				// tu T1

				lcd_locate(2,0);
				lcd_str("T2=");
				lcd_int(temperature_2);
				lcd_str("   ");
				lcd_locate(2,6);
				lcd_str("              ");
				// tu T2

				lcd_locate(3,0);
				show_day_of_week(dayweek);
				lcd_str(", ");
				if(day < 10) lcd_int(0);
				lcd_int(day);
				lcd_str(" ");
				show_month(month);
				lcd_str(" ");
				if(hour < 10) lcd_int(0);
				lcd_int(hour);
				lcd_str(":");
				if(min < 10) lcd_int(0);
				lcd_int(min);
				lcd_str(":");
				if(sec < 10) lcd_int(0);
				lcd_int(sec);
				break;

		case 2:
				switch(menu_cnt)
				{
					case 1: lcd_locate(0,0);
							lcd_char('>');
							display_menu_1;
							lcd_locate(1,0);
							lcd_char('-');
							display_menu_2;
							lcd_locate(2,0);
							lcd_char('-');
							display_menu_3;
							lcd_locate(3,0);
							lcd_char('-');
							display_menu_4;
							break;
					case 2: lcd_locate(0,0);
							lcd_char('-');
							display_menu_1;
							lcd_locate(1,0);
							lcd_char('>');
							display_menu_2;
							lcd_locate(2,0);
							lcd_char('-');
							display_menu_3;
							lcd_locate(3,0);
							lcd_char('-');
							display_menu_4;
							break;
					case 3: lcd_locate(0,0);
							lcd_char('-');
							display_menu_1;
							lcd_locate(1,0);
							lcd_char('-');
							display_menu_2;
							lcd_locate(2,0);
							lcd_char('>');
							display_menu_3;
							lcd_locate(3,0);
							lcd_char('-');
							display_menu_4;
							break;
					case 4: lcd_locate(0,0);
							lcd_char('-');
							display_menu_1;
							lcd_locate(1,0);
							lcd_char('-');
							display_menu_2;
							lcd_locate(2,0);
							lcd_char('-');
							display_menu_3;
							lcd_locate(3,0);
							lcd_char('>');
							display_menu_4;
							break;
				}
				break;

		case 3:
				switch(menu_cnt)
				{
					case 1: lcd_locate(0,0);
							lcd_str("hh:  <-             ");
							lcd_locate(1,0);
							lcd_str("mm:                 ");
							lcd_locate(2,0);
							lcd_str("ss:                 ");
							lcd_locate(3,0);
							display_empty_line;
							break;
					case 2: lcd_locate(0,0);
							lcd_str("d.tyg: <-           ");
							lcd_locate(1,0);
							lcd_str("dzien:              ");
							lcd_locate(2,0);
							lcd_str("mies:               ");
							lcd_locate(3,0);
							lcd_str("rok:                ");
							break;
					case 3: lcd_str("STATUS ALARMU:      ");
							lcd_locate(1,0);
							if(alarm_flag & 0x02) lcd_str("Rozbrojony          ");
							else lcd_str("Uzbrojony           ");
							lcd_locate(2,0);
							display_empty_line;
							lcd_locate(3,0);
							display_empty_line;
							break;
					case 4: if(alarm_menu_cnt < 5) {
								for(uint8_t i = 0; i < 4; i++)
								{
									lcd_locate(i,0);
									if(alarm_menu_cnt == i + 1) lcd_char('>');
									else lcd_char('-');
									if(alarm_1_buff[i][0] == 0) {
										lcd_str("Brak alarmu 1: ");
										lcd_int(i+1);
									}
									else {
										lcd_str("Zapisany alarm 1:");
										lcd_int(i+1);
									}
								}
							}
							else {
								for(uint8_t i = 0; i < 4; i++)
								{
									lcd_locate(i,0);
									if(alarm_menu_cnt == i + 5) lcd_char('>');
									else lcd_char('-');
									if(alarm_2_buff[i][0] == 0) {
										lcd_str("Brak alarmu 2: ");
										lcd_int(i+1);
									}
									else {
										lcd_str("Zapisany alarm 2:");
										lcd_int(i+1);
									}
								}
							}
							break;
				}
				break;
			}
		refresh_lcd = 0;
	}
}
