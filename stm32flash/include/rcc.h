// Header for Reset and Clock Control
//
// Refer to rm0316 as some bits can only be set per device


#ifndef RCC_H
#define RCC_H

#include <stdint.h>


#define HSION (1<<8)
#define HSIRDY (1<<10)

#define RCC_HSION (1<<8)
#define RCC_HSIRDY (1<<10)

#define RCC_PLLON 1 <<24
#define RCC_PLLRDY 1 <<25

#define RCC_SW_PLL 3
#define RCC_SWS_PLL 3<<2

#define RCC_PLLSRC_HSI 2
#define RCC_PLLCFGR_PLLN_8 8<<8

#define RCC_PLLREN 1<<24

#define RCC_PPRE1_DIV8 6<<8
#define RCC_PPRE1_DIV16 7<<8

#define DMA1_EN (1<<0)
#define DMA2_EN (1<<1)

#define GPIOA_EN (1<<0)
#define GPIOB_EN (1<<1)

#define TIM1_EN  (1<<11)
#define TIM2_EN  (1<<0)
#define TIM15_EN (1<<16)
#define TIM16_EN (1<<17)

#define USART1_EN (1<<14)

#define SPI1_EN (1<<12)

typedef struct{
    volatile uint32_t CR;       //0x00
    volatile uint32_t ICSCR;    //0x04
    volatile uint32_t CFGR;     //0x08
    volatile uint32_t PLLCFGR;  //0x0C
    volatile uint32_t RESERVE[2];
    volatile uint32_t CIER;      //0x18
    volatile uint32_t CIFR;      //0x1C
    volatile uint32_t CICR;     //0x20
    volatile uint32_t RESERVE2; //0x24
    volatile uint32_t AHB1RSTR;   //0x28
    volatile uint32_t AHB2RSTR;   //0x2C
    volatile uint32_t AHB3RSTR;   //0x30
    volatile uint32_t RESERVE3; //0x34
    volatile uint32_t APB1RSTR1; //0x38
    volatile uint32_t APB1RSTR2; //0x3C
    volatile uint32_t APB2RSTR; //0x40
    volatile uint32_t RESERVEd; //0x44
    volatile uint32_t AHB1ENR;   //0x48
    volatile uint32_t AHB2ENR;  //0x4C
    volatile uint32_t AHB3ENR;  //0x50
    volatile uint32_t RESERVE4;  //0x54
    volatile uint32_t APB1ENR1;     //0x58
    volatile uint32_t APB1ENR2;     //0x5C
    volatile uint32_t APB2ENR;  //0x60
    volatile uint32_t RESERVE5;  //0x64
    volatile uint32_t AHB1SMENR; //0x68
    volatile uint32_t AHB2SMENR; //0x6C
    volatile uint32_t AHB3SMENR;    //0x70
    volatile uint32_t RESERVE6;  //0x74
    volatile uint32_t APB1SMENR1; //0x78
    volatile uint32_t APB1SMENR2;   //0x7C
    volatile uint32_t APB2SMENR;    //0x80
    volatile uint32_t RESERVE7;  //0x84
    volatile uint32_t CCIPR;    //0x88
    volatile uint32_t RESERVE8; //0x8C
    volatile uint32_t BDCR;     //0x90
    volatile uint32_t CSR;      //0x94
    volatile uint32_t CRRCR;    //0x98
    volatile uint32_t CCIPR2;   //0x9C
} RCC_TypeDef;

#define RCC ((RCC_TypeDef *) 0x40021000)

void set_clock_128Mhz(void);

#endif
