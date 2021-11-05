/*
 * SPI_lib.h
 *
 *  Created on: 29 cze 2020
 *      Author: Dominik
 */

#ifndef SPI_LIB_H_
#define SPI_LIB_H_

#define USE_SPI_2_SOFT 1


void InitSoftSpi(void);
void SendSoftSpi(uint8_t bajt);

#if USE_SPI_2_SOFT == 1
void SendSoftSpi2(uint8_t bajt);
#endif

#endif /* SPI_LIB_H_ */
