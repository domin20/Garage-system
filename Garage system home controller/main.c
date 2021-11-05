/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "kn4x4.h"
#include "commands.h"
#include "radio.h"
#include "lcd.h"
#include "SPI_lib.h"
#include "interface.h"
#include "ds3231.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint16_t ms1 = 0, cnt = 0;
uint8_t s1 = 0;
uint8_t ms500 = 0;
uint8_t ms50 = 0;
uint8_t ms200 = 0;

volatile uint8_t radio_buffer[20];
volatile uint8_t radio_flag = 0;
volatile uint8_t byte = 0;

uint16_t temp_buff[5];
uint8_t temp_cnt = 0;
int8_t room_temperature = 0;
int8_t temperature_1 = 0, temperature_2 = 0;

uint8_t state_sensors_1 = 0, state_sensors_2 = 0;
uint8_t register_1 = 0, register_2 = 0;

uint8_t time_buff[7];
uint8_t sec, min, hour, day, month, dayweek;
uint16_t year;

uint8_t alarm_flag = 0;
uint8_t alarm_buff_flag = 0;
uint8_t alarm_1_buff[4][8];
uint8_t alarm_2_buff[4][8];
uint8_t alarm_menu_cnt = 1;

uint8_t state = 1;
uint8_t refresh_lcd = 0;
uint16_t cnt_for_animation = 0;
uint8_t menu_cnt = 1;
char key = 0;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADC_Stop(&hadc1);
  HAL_ADCEx_Calibration_Start(&hadc1);
  HAL_UART_Receive_IT(&huart2, (uint8_t*)&byte, 1);

  lcd_init();
  InitSoftSpi();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(ms500 != ms1/500)
	  {
		  // ------- pomiar temperatury --------
		  HAL_ADC_Start(&hadc1);
		  while(HAL_ADC_PollForConversion(&hadc1,1) != HAL_OK);
		  temp_buff[temp_cnt] = HAL_ADC_GetValue(&hadc1);
		  temp_cnt++;
		  if(temp_cnt > 4) temp_cnt = 0;
		  uint16_t average = 0;
		  for(uint8_t i = 0; i < 5; i++)
		  {
			  average += temp_buff[i];
		  }
		  average = 4095 - average;
		  average /= 20;
		  room_temperature = average / 5;	// regulacja dzielnika
		  room_temperature -= 90;	// offset


		  // ------obsługa RTC tj. pobranie daty i godziny oraz wyswietlenie-------
		  HAL_I2C_Mem_Read_IT(&hi2c1, 0x68<<1, 0x00, 1, time_buff, 7);	// pobieranie czasu z RTC

		  sec=bcd2dec(time_buff[0]);
		  min=bcd2dec(time_buff[1]);
		  hour=bcd2dec(time_buff[2]);
		  dayweek=bcd2dec(time_buff[3]);
		  day=bcd2dec(time_buff[4]);
		  month=bcd2dec(time_buff[5]);
		  year=bcd2dec(time_buff[6]);
		  year=year+2000;

		  // -------- ODPAL ALARM --------

		  if(alarm_flag) register_1 |= 0x04;
		  else register_1 &= ~0x04;

		  SendSoftSpi(register_1);
		  refresh_lcd = 1;

		  ms500 = ms1/500;
	  }

	  if(radio_flag & 0x01)		// obsługa odebranej ramki danych
	  {
	  	  execute_command((uint8_t*)radio_buffer);
	  	  radio_flag &= ~0x01;
	  	  cnt = 0;
	  }

	  // -------- jesli dane nie nadeszly przez ostatnie 5s to zalacz diode oznaczajaca brak nadchodzacych danych ---------
	  if(cnt > 5000) {register_2 |= (0x01 << 5); SendSoftSpi2(register_2);}

	  if((hour >= 22 || hour <= 6) && (alarm_flag & 0x02) == 0)	// ustawienie flagi alarmu, jesli alarm jest uzbrojony
	  {
		  if(state_sensors_1 & 0x01) {
			  alarm_flag |= 0x01;
			  uint8_t i = 0;
			  if((alarm_buff_flag & 0b1) == 0) i = 0;
			  else if((alarm_buff_flag & 0b10) == 0) i = 1;
			  else if((alarm_buff_flag & 0b100) == 0) i = 2;
			  else if((alarm_buff_flag & 0b1000) == 0) i = 3;
			  {
				  alarm_1_buff[i][0] = dayweek;
				  alarm_1_buff[i][1] = day;
				  alarm_1_buff[i][2] = month;
				  alarm_1_buff[i][3] = year - 2000;
				  alarm_1_buff[i][4] = hour;
				  alarm_1_buff[i][5] = min;
				  alarm_1_buff[i][6] = sec;
				  alarm_1_buff[i][7] = state_sensors_2;
			  }
		  }
		  else alarm_flag &= ~0x01;

		  if(state_sensors_2 & 0x03) {
			  alarm_flag |= 0x01;
			  uint8_t i = 0;
			  if((alarm_buff_flag & 0b10000) == 0) i = 0;
			  else if((alarm_buff_flag & 0b100000) == 0) i = 1;
			  else if((alarm_buff_flag & 0b1000000) == 0) i = 2;
			  else if((alarm_buff_flag & 0b10000000) == 0) i = 3;
			  {
				  alarm_2_buff[i][0] = dayweek;
			  	  alarm_2_buff[i][1] = day;
			  	  alarm_2_buff[i][2] = month;
			  	  alarm_2_buff[i][3] = year - 2000;
			  	  alarm_2_buff[i][4] = hour;
			  	  alarm_2_buff[i][5] = min;
			  	  alarm_2_buff[i][6] = sec;
			  	  alarm_2_buff[i][7] = state_sensors_2;
			  }
		  }
		  else alarm_flag &= ~0x01;
	  }
	  else alarm_flag &= ~0x01;

	  if(ms50 != ms1 / 50)
	  {
		  getc_from_keyboard(&key);
		  if(key)
		  {
			  execute_kn4x4_command(key);
			  key = 0;
			  //execute_kn4x4_command(key);
		  }
		  ms50 = ms1 / 50;
	  }

	  if(refresh_lcd) refresh_display();	// glowne odswiezanie wyswietlacza


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2){
		get_radio_frame();
	}
	HAL_UART_Receive_IT(&huart2, (uint8_t*)&byte, 1);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
