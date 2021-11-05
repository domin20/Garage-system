/*
 * SPI_lib.c
 *
 *  Created on: 29 cze 2020
 *      Author: Dominik
 */
#include "main.h"
#include "SPI_lib.h"


#define SCK_0 HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_RESET)
#define SCK_1 HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_SET)
#define MOSI_0 HAL_GPIO_WritePin(MOSI_GPIO_Port, MOSI_Pin, GPIO_PIN_RESET)
#define MOSI_1 HAL_GPIO_WritePin(MOSI_GPIO_Port, MOSI_Pin, GPIO_PIN_SET)
#define CS_0 HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET)
#define CS_1 HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET)


#if USE_SPI_2_SOFT == 1

#define SCK_2_0 HAL_GPIO_WritePin(SCK_2_GPIO_Port, SCK_2_Pin, GPIO_PIN_RESET)
#define SCK_2_1 HAL_GPIO_WritePin(SCK_2_GPIO_Port, SCK_2_Pin, GPIO_PIN_SET)
#define MOSI_2_0 HAL_GPIO_WritePin(MOSI_2_GPIO_Port, MOSI_2_Pin, GPIO_PIN_RESET)
#define MOSI_2_1 HAL_GPIO_WritePin(MOSI_2_GPIO_Port, MOSI_2_Pin, GPIO_PIN_SET)
#define CS_2_0 HAL_GPIO_WritePin(CS_2_GPIO_Port, CS_2_Pin, GPIO_PIN_RESET)
#define CS_2_1 HAL_GPIO_WritePin(CS_2_GPIO_Port, CS_2_Pin, GPIO_PIN_SET)

#endif



void InitSoftSpi(void)
{
	if(MOSI_Pin < 8) MOSI_GPIO_Port->CRL |= (0x2 << MOSI_Pin*4);
	else MOSI_GPIO_Port->CRH |= (0x2 << MOSI_Pin*4);

	if(SCK_Pin < 8) SCK_GPIO_Port->CRL |= (0x2 << SCK_Pin*4);
	else SCK_GPIO_Port->CRH |= (0x2 << SCK_Pin*4);

	if(CS_Pin < 8) CS_GPIO_Port->CRL |= (0x2 << CS_Pin*4);
	else CS_GPIO_Port->CRH |= (0x2 << CS_Pin*4);

#if USE_SPI_2_SOFT == 1
	if(MOSI_2_Pin < 8) MOSI_2_GPIO_Port->CRL |= (0x2 << MOSI_2_Pin*4);
	else MOSI_2_GPIO_Port->CRH |= (0x2 << MOSI_2_Pin*4);

	if(SCK_2_Pin < 8) SCK_2_GPIO_Port->CRL |= (0x2 << SCK_2_Pin*4);
	else SCK_2_GPIO_Port->CRH |= (0x2 << SCK_2_Pin*4);

	if(CS_2_Pin < 8) CS_2_GPIO_Port->CRL |= (0x2 << CS_2_Pin*4);
	else CS_2_GPIO_Port->CRH |= (0x2 << CS_2_Pin*4);
#endif

	//DDRB |= (1<<SMOSI) | (1<<SSCK) | (1<<SCS);
}
void SendSoftSpi(uint8_t bajt)
{
	uint8_t cnt = 0x80;

	SCK_0;

	while(cnt)
	{
		if(bajt & cnt) MOSI_1;
		else MOSI_0;
		SCK_1;
		SCK_0;

		cnt >>= 1;
	}

	CS_1;
	CS_0;
}

#if USE_SPI_2_SOFT == 1

void SendSoftSpi2(uint8_t bajt)
{
	uint8_t cnt = 0x80;

	SCK_2_0;

	while(cnt)
	{
		if(bajt & cnt) MOSI_2_1;
		else MOSI_2_0;
		SCK_2_1;
		SCK_2_0;

		cnt >>= 1;
	}

	CS_2_1;
	CS_2_0;
}
#endif

