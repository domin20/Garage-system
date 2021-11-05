/*
 * kn4x4.c
 *
 *  Created on: May 14, 2021
 *      Author: Dominik
 */
#include "kn4x4.h"
#include "main.h"

void getc_from_keyboard(char * c)
{
	static uint8_t wait_for_rel_button = 0;

	if(!wait_for_rel_button)
	{
		ActivateCol1;
		if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_SET) {wait_for_rel_button = 1; (*c) = '1';}
		if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_SET) {wait_for_rel_button = 1; (*c) = '4';}
		if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_SET) {wait_for_rel_button = 1; (*c) = '7';}
		if(HAL_GPIO_ReadPin(ROW4_GPIO_Port, ROW4_Pin) == GPIO_PIN_SET) {wait_for_rel_button = 1; (*c) = '*';}
		DeactivateCol1;
	}

	if(!wait_for_rel_button)
	{
		ActivateCol2;
		if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_SET) {wait_for_rel_button = 1; (*c) = '2';}
		if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_SET) {wait_for_rel_button = 1; (*c) = '5';}
		if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_SET) {wait_for_rel_button = 1; (*c) = '8';}
		if(HAL_GPIO_ReadPin(ROW4_GPIO_Port, ROW4_Pin) == GPIO_PIN_SET) {wait_for_rel_button = 1; (*c) = '0';}
		DeactivateCol2;
	}

	if(!wait_for_rel_button)
	{
		ActivateCol3;
		if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_SET) {wait_for_rel_button = 1; (*c) = '3';}
		if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_SET) {wait_for_rel_button = 1; (*c) = '6';}
		if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_SET) {wait_for_rel_button = 1; (*c) = '9';}
		if(HAL_GPIO_ReadPin(ROW4_GPIO_Port, ROW4_Pin) == GPIO_PIN_SET) {wait_for_rel_button = 1; (*c) = '#';}
		DeactivateCol3;
	}

	if(!wait_for_rel_button)
	{
		ActivateCol4;
		if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_SET) {wait_for_rel_button = 1; (*c) = 'A';}
		if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_SET) {wait_for_rel_button = 1; (*c) = 'B';}
		if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_SET) {wait_for_rel_button = 1; (*c) = 'C';}
		if(HAL_GPIO_ReadPin(ROW4_GPIO_Port, ROW4_Pin) == GPIO_PIN_SET) {wait_for_rel_button = 1; (*c) = 'D';}
		DeactivateCol4;
	}

	ActivateCol1;
	ActivateCol2;
	ActivateCol3;
	ActivateCol4;

	// ustawienie indywidualne...
	//if(((GPIOA->IDR & 0b1100010) == 0) && ((GPIOC->IDR & 0b10000) == 0)) wait_for_rel_button = 0;

	if(((ROW1_GPIO_Port->IDR & ROW1_Pin) == 0) && ((ROW2_GPIO_Port->IDR & ROW2_Pin) == 0) &&
			((ROW3_GPIO_Port->IDR & ROW3_Pin) == 0) && ((ROW4_GPIO_Port->IDR & ROW4_Pin) == 0)) wait_for_rel_button = 0;

	DeactivateCol1;
	DeactivateCol2;
	DeactivateCol3;
	DeactivateCol4;
}


