/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern TIM_HandleTypeDef htim1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void TIM1_Init(void);


//使用Tim1定时器实现微妙延时

#define DLY_TIM_Handle (&htim1)
#define hal_delay_us(nus) do { \
    __HAL_TIM_SET_COUNTER(DLY_TIM_Handle, 0); \
    __HAL_TIM_ENABLE(DLY_TIM_Handle); \
    while (__HAL_TIM_GET_COUNTER(DLY_TIM_Handle) < (nus)); \
    __HAL_TIM_DISABLE(DLY_TIM_Handle); \
} while(0)


#define us_delay(t) hal_delay_us(t)


#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

