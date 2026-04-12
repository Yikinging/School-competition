/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f4xx_hal.h"

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
#define VAC_Pin GPIO_PIN_0
#define VAC_GPIO_Port GPIOC
#define IAC_Pin GPIO_PIN_1
#define IAC_GPIO_Port GPIOC
#define VDC_Pin GPIO_PIN_2
#define VDC_GPIO_Port GPIOC
#define IDC_Pin GPIO_PIN_3
#define IDC_GPIO_Port GPIOC
#define VBUS_Pin GPIO_PIN_0
#define VBUS_GPIO_Port GPIOA
#define SW2_Pin GPIO_PIN_3
#define SW2_GPIO_Port GPIOA
#define SW1_Pin GPIO_PIN_4
#define SW1_GPIO_Port GPIOA
#define EncoderD_Pin GPIO_PIN_5
#define EncoderD_GPIO_Port GPIOA
#define EncoderA_Pin GPIO_PIN_6
#define EncoderA_GPIO_Port GPIOA
#define EncoderB_Pin GPIO_PIN_7
#define EncoderB_GPIO_Port GPIOA
#define L1_Pin GPIO_PIN_8
#define L1_GPIO_Port GPIOE
#define H1_Pin GPIO_PIN_9
#define H1_GPIO_Port GPIOE
#define L3_Pin GPIO_PIN_10
#define L3_GPIO_Port GPIOE
#define H3_Pin GPIO_PIN_11
#define H3_GPIO_Port GPIOE
#define L2_Pin GPIO_PIN_12
#define L2_GPIO_Port GPIOE
#define H2_Pin GPIO_PIN_13
#define H2_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
