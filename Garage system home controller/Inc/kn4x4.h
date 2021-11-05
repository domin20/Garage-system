/*
 * kn4x4.h
 *
 *  Created on: May 14, 2021
 *      Author: Dominik
 */

#ifndef INC_KN4X4_H_
#define INC_KN4X4_H_


#define ActivateCol1 HAL_GPIO_WritePin(COL1_GPIO_Port, COL1_Pin, GPIO_PIN_SET)
#define ActivateCol2 HAL_GPIO_WritePin(COL2_GPIO_Port, COL2_Pin, GPIO_PIN_SET)
#define ActivateCol3 HAL_GPIO_WritePin(COL3_GPIO_Port, COL3_Pin, GPIO_PIN_SET)
#define ActivateCol4 HAL_GPIO_WritePin(COL4_GPIO_Port, COL4_Pin, GPIO_PIN_SET)

#define DeactivateCol1 HAL_GPIO_WritePin(COL1_GPIO_Port, COL1_Pin, GPIO_PIN_RESET)
#define DeactivateCol2 HAL_GPIO_WritePin(COL2_GPIO_Port, COL2_Pin, GPIO_PIN_RESET)
#define DeactivateCol3 HAL_GPIO_WritePin(COL3_GPIO_Port, COL3_Pin, GPIO_PIN_RESET)
#define DeactivateCol4 HAL_GPIO_WritePin(COL4_GPIO_Port, COL4_Pin, GPIO_PIN_RESET)



void getc_from_keyboard(char * c);


#endif /* INC_KN4X4_H_ */
