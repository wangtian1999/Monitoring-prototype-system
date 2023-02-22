#include "stm32f0xx_hal.h"
#include "stdlib.h"
#include "stdio.h"

uint32_t time = 0;

/*
*brif:自定义延迟函数
*/
void MyDelay(uint32_t nTime)
{
    uint16_t i,j;
    for (i=0;i<nTime;i++)
    {
        for (j=0;j<10000;j++)
            {}
    }
}


/*
*brif:蜂鸣器状态提示
*
*/
void TIP_SUCCESS(void) //读卡成功 短声1次
{
    GPIOA->BRR = GPIO_PIN_0;
    MyDelay(25);
    GPIOA->BSRR = GPIO_PIN_0;
}

void TIP_FAILED(void) //读卡失败 长声1次
{
    GPIOA->BRR = GPIO_PIN_0;
    MyDelay(70);
    GPIOA->BSRR = GPIO_PIN_0;
}

void TIP_WRITE_SUCCESS(void) //开机提示音 短声2次
{
    GPIOA->BRR = GPIO_PIN_0;
    MyDelay(25);
    GPIOA->BSRR = GPIO_PIN_0;
    MyDelay(35);
    GPIOA->BRR = GPIO_PIN_0;
    MyDelay(25);
    GPIOA->BSRR = GPIO_PIN_0;
}

void TIP_WRITE_FAILED(void)
{
    GPIOA->BRR = GPIO_PIN_0;
    MyDelay(25);
    GPIOA->BSRR = GPIO_PIN_0;
    MyDelay(35);
    GPIOA->BRR = GPIO_PIN_0;
    MyDelay(70);
    GPIOA->BSRR = GPIO_PIN_0;
}

void TIP_DELATE_ALL(void)
{
    GPIOA->BRR = GPIO_PIN_0;
    MyDelay(70);
    GPIOA->BSRR = GPIO_PIN_0;
    MyDelay(35);
    GPIOA->BRR = GPIO_PIN_0;
    MyDelay(25);
    GPIOA->BSRR = GPIO_PIN_0;
}


