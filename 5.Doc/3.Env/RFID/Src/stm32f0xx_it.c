/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "stm32f0xx_it.h"

/* USER CODE BEGIN 0 */
#include "RC522.h"
#include "i2c.h"

#define BUFFER_SIZE 128
uint8_t rx_len;
uint8_t recv_end_flag;
uint8_t rx_buffer[128];
uint8_t Data_Flag = 0;


uint8_t Card_Data[30];
uint8_t I2C_FLAG = 0;
uint8_t CardDateNum = 12;
static uint16_t RestBusTime = 0;
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim14;

extern DMA_HandleTypeDef hdma_usart1_rx;
extern UART_HandleTypeDef huart1;

/******************************************************************************/
/*            Cortex-M0 Processor Interruption and Exception Handlers         */
/******************************************************************************/

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
    /* USER CODE BEGIN SysTick_IRQn 0 */

    /* USER CODE END SysTick_IRQn 0 */
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
    /* USER CODE BEGIN SysTick_IRQn 1 */

    /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles EXTI line 0 and 1 interrupts.
*/
void EXTI0_1_IRQHandler(void)
{
    /* USER CODE BEGIN EXTI0_1_IRQn 0 */

    /* USER CODE END EXTI0_1_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
    /* USER CODE BEGIN EXTI0_1_IRQn 1 */

    /* USER CODE END EXTI0_1_IRQn 1 */
}

/**
* @brief This function handles TIM14 global interrupt.
*/
void TIM14_IRQHandler(void)
{
    /* USER CODE BEGIN TIM14_IRQn 0 */

    /* USER CODE END TIM14_IRQn 0 */
    HAL_TIM_IRQHandler(&htim14);
    /* USER CODE BEGIN TIM14_IRQn 1 */

    /* USER CODE END TIM14_IRQn 1 */
}
void USART1_IRQHandler(void)
{
 //   printf("rev\n");
    HAL_UART_IRQHandler(&huart1);
}
/**
* @brief This function handles I2C1 event global interrupt / I2C1 wake-up interrupt through EXTI line 23.
*/
void I2C1_IRQHandler(void)
{
    /* USER CODE BEGIN I2C1_IRQn 0 */
    if (__HAL_I2C_GET_FLAG(&hi2c1,I2C_IT_ADDRI) == SET)//检测地址是否匹配
    {
        __HAL_I2C_CLEAR_FLAG(&hi2c1, I2C_IT_ADDRI);//地址匹配后清除标志位
        if (__HAL_I2C_GET_FLAG(&hi2c1,I2C_ISR_DIR) == SET)//检测传输方向(此处从机发送数据)
        {
            __HAL_I2C_ENABLE_IT(&hi2c1,I2C_IT_TXI);//开启数据发送中断
        }
    }
    else if (__HAL_I2C_GET_FLAG(&hi2c1,I2C_IT_TXI) == SET)//检测是否为发送中断
    {
        hi2c1.Instance->TXDR = Card_Data[CardDateNum];
        CardDateNum++;

        if (CardDateNum >= 16)
        {
            CardDateNum = 12;
            __HAL_I2C_DISABLE_IT(&hi2c1,I2C_IT_TXI);
        }
    }

    /* USER CODE END I2C1_IRQn 0 */
    if (hi2c1.Instance->ISR & (I2C_FLAG_BERR | I2C_FLAG_ARLO | I2C_FLAG_OVR)) {
        HAL_I2C_ER_IRQHandler(&hi2c1);
    } else {
        HAL_I2C_EV_IRQHandler(&hi2c1);
    }
    /* USER CODE BEGIN I2C1_IRQn 1 */

    /* USER CODE END I2C1_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim14)
    {
        printf("tim14 irq in\n");
    }
}

void HAL_SYSTICK_Callback(void)
{
    //2s复位一次I2C总线
    RestBusTime++;
    if ((RestBusTime >= 2000) && ((__HAL_I2C_GET_FLAG(&hi2c1,I2C_FLAG_BUSY) != SET) || (__HAL_I2C_GET_FLAG(&hi2c1,I2C_FLAG_BERR) == SET)))
    {
        RestBusTime = 0;
        MX_I2C1_Init();
        __HAL_I2C_ENABLE_IT(&hi2c1,I2C_IT_ADDRI);
    }
}
/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
