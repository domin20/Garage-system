/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define COL1_Pin GPIO_PIN_4
#define COL1_GPIO_Port GPIOA
#define COL2_Pin GPIO_PIN_5
#define COL2_GPIO_Port GPIOA
#define COL3_Pin GPIO_PIN_6
#define COL3_GPIO_Port GPIOA
#define COL4_Pin GPIO_PIN_7
#define COL4_GPIO_Port GPIOA
#define ROW1_Pin GPIO_PIN_0
#define ROW1_GPIO_Port GPIOB
#define ROW2_Pin GPIO_PIN_1
#define ROW2_GPIO_Port GPIOB
#define ROW3_Pin GPIO_PIN_10
#define ROW3_GPIO_Port GPIOB
#define ROW4_Pin GPIO_PIN_11
#define ROW4_GPIO_Port GPIOB
#define MOSI_2_Pin GPIO_PIN_12
#define MOSI_2_GPIO_Port GPIOB
#define CS_2_Pin GPIO_PIN_14
#define CS_2_GPIO_Port GPIOB
#define SCK_2_Pin GPIO_PIN_15
#define SCK_2_GPIO_Port GPIOB
#define E_Pin GPIO_PIN_8
#define E_GPIO_Port GPIOA
#define RS_Pin GPIO_PIN_9
#define RS_GPIO_Port GPIOA
#define D4_Pin GPIO_PIN_10
#define D4_GPIO_Port GPIOA
#define D5_Pin GPIO_PIN_11
#define D5_GPIO_Port GPIOA
#define D6_Pin GPIO_PIN_12
#define D6_GPIO_Port GPIOA
#define D7_Pin GPIO_PIN_15
#define D7_GPIO_Port GPIOA
#define MOSI_Pin GPIO_PIN_3
#define MOSI_GPIO_Port GPIOB
#define CS_Pin GPIO_PIN_4
#define CS_GPIO_Port GPIOB
#define SCK_Pin GPIO_PIN_5
#define SCK_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
