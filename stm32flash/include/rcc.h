// Header for Reset and Clock Control
//
// Refer to rm0316 as some bits can only be set per device


#ifndef RCC_H
#define RCC_H

#include <stdint.h>

#define FCLK 8000000

#define DMA1_EN (1<<0)
#define DMA2_EN (1<<1)

#define GPIOA_EN (1<<17)
#define GPIOB_EN (1<<18)
#define GPIOC_EN (1<<19)
#define GPIOD_EN (1<<20)

#define TIM1_EN  (1<<11)
#define TIM8_EN  (1<<13)
#define TIM15_EN (1<<16)
#define TIM16_EN (1<<17)
#define TIM17_EN (1<<18)
#define TIM20_EN (1<<20)

#define USART1_EN (1<<14)
#define USART2_EN (1<<17)
#define USART3_EN (1<<18)
#define UART4_EN  (1<<19)
#define UART5_EN  (1<<20)

typedef struct{
    volatile uint32_t CR;       //0x00
    volatile uint32_t CFGR;     //0x04
    volatile uint32_t CIR;      //0x08
    volatile uint32_t APB2RSTR; //0X0C
    volatile uint32_t APB1RSTR; //0x10
    volatile uint32_t AHBENR;   //0x14
    volatile uint32_t APB2ENR;  //0x18
    volatile uint32_t APB1ENR;  //0x1C
    volatile uint32_t BDCR;     //0x20
    volatile uint32_t CSR;      //0x24
    volatile uint32_t AHBRSTR;  //0x28
    volatile uint32_t CFGR2;    //0x2C
    volatile uint32_t CFGR3;    //0x30
} RCC_TypeDef;

#define RCC ((RCC_TypeDef *) 0x40021000)

// only supports one timer 
void TimerEnable(uint8_t timer);

#endif
