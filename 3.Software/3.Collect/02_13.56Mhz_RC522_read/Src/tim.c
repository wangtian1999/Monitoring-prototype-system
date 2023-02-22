/**
  ******************************************************************************
  * File Name          : TIM.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

TIM_HandleTypeDef htim14;
TIM_HandleTypeDef htim16;

/* TIM14 init function */
void MX_TIM14_Init(void)
{

    htim14.Instance = TIM14;
    htim14.Init.Prescaler = 2000;
    htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim14.Init.Period = 200;
    htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&htim14);

}
/* TIM16 init function */
void MX_TIM16_Init(void)
{

    htim16.Instance = TIM16;
    htim16.Init.Prescaler = 2000;
    htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim16.Init.Period = 100;
    htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim16.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&htim16);

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{

    if (htim_base->Instance==TIM14)
    {
        /* USER CODE BEGIN TIM14_MspInit 0 */

        /* USER CODE END TIM14_MspInit 0 */
        /* Peripheral clock enable */
        __TIM14_CLK_ENABLE();

        /* Peripheral interrupt init*/
        HAL_NVIC_SetPriority(TIM14_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM14_IRQn);
        /* USER CODE BEGIN TIM14_MspInit 1 */

        /* USER CODE END TIM14_MspInit 1 */
    }
    else if (htim_base->Instance==TIM16)
    {
        /* USER CODE BEGIN TIM16_MspInit 0 */

        /* USER CODE END TIM16_MspInit 0 */
        /* Peripheral clock enable */
        __TIM16_CLK_ENABLE();
        /* USER CODE BEGIN TIM16_MspInit 1 */

        /* USER CODE END TIM16_MspInit 1 */
    }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{

    if (htim_base->Instance==TIM14)
    {
        /* USER CODE BEGIN TIM14_MspDeInit 0 */

        /* USER CODE END TIM14_MspDeInit 0 */
        /* Peripheral clock disable */
        __TIM14_CLK_DISABLE();

        /* Peripheral interrupt Deinit*/
        HAL_NVIC_DisableIRQ(TIM14_IRQn);

        /* USER CODE BEGIN TIM14_MspDeInit 1 */

        /* USER CODE END TIM14_MspDeInit 1 */
    }
    else if (htim_base->Instance==TIM16)
    {
        /* USER CODE BEGIN TIM16_MspDeInit 0 */

        /* USER CODE END TIM16_MspDeInit 0 */
        /* Peripheral clock disable */
        __TIM16_CLK_DISABLE();
        /* USER CODE BEGIN TIM16_MspDeInit 1 */

        /* USER CODE END TIM16_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
