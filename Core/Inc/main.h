/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define RX_BUFF_SIZE 255
#define TX_BUFF_SIZE 255
#define ERR_ARRAY_SIZE 255


struct comm_data{
  UART_HandleTypeDef *plotter_handle;
  UART_HandleTypeDef *flasher_handle;
  uint8_t plotter_rx[RX_BUFF_SIZE];
  uint8_t plotter_tx[TX_BUFF_SIZE];
  uint8_t flasher_rx[RX_BUFF_SIZE];
  uint8_t flasher_tx[TX_BUFF_SIZE];
};

enum execution_status{
  Movement,
  Flash_pending,
  Flash_finished, 
  Sequence_finished,
  Idle,
  Fail};

struct setup_data{
  enum execution_status exec_status;
  uint16_t offset_x;
  uint16_t offset_y;
  uint8_t flash_matrix_dims[2];
  uint8_t flash_matrix_pos[2];
  char fail_msg[ERR_ARRAY_SIZE];
};


/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define RETURN_OK 0
#define RETURN_FAIL 1

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define select_Pin GPIO_PIN_0
#define select_GPIO_Port GPIOB
#define cancel_Pin GPIO_PIN_1
#define cancel_GPIO_Port GPIOB
#define buzzer_Pin GPIO_PIN_11
#define buzzer_GPIO_Port GPIOA
#define st7735_res_Pin GPIO_PIN_6
#define st7735_res_GPIO_Port GPIOB
#define st7735_dc_Pin GPIO_PIN_7
#define st7735_dc_GPIO_Port GPIOB
#define st7735_cs_Pin GPIO_PIN_8
#define st7735_cs_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
