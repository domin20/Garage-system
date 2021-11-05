/*
 * interface.h
 *
 *  Created on: May 16, 2021
 *      Author: Dominik
 */

#ifndef INC_INTERFACE_H_
#define INC_INTERFACE_H_

#define display_menu_1 lcd_str("Ustaw czas         ")
#define display_menu_2 lcd_str("Ustaw date         ")
#define display_menu_3 lcd_str("Status alarmu      ")
#define display_menu_4 lcd_str("Zgloszone alarmy   ")
#define display_empty_line lcd_str("                    ")

void execute_kn4x4_command(char c);
void create_new_time(char c);
void create_new_date(char c);
void show_saved_alarms(void);
void handling_saved_alarms(void);
void refresh_display(void);


#endif /* INC_INTERFACE_H_ */
