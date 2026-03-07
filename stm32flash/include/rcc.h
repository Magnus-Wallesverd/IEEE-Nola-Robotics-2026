// Header for Reset and Clock Control
//
// Refer to rm0316 as some bits can only be set per device


#ifndef RCC_H
#define RCC_H

#include <stdint.h>

#define FCLK 8000000

#define CLK_8Mhz   8000000
#define CLK_16Mhz 16000000
#define CLK_32Mhz 32000000
#define CLK_64Mhz 64000000

#define RCC_HSION (1<<0)
#define RCC_HSIRDY (1<<1)

#define RCC_PLLON   (1<<24)
#define RCC_PLLRDY  (1<<25)

#define RCC_PLL_8x      (6<<18)

#define RCC_PPRE2_DIV2  (4<<11)
#define RCC_PPRE2_DIV4  (5<<11)
#define RCC_PPRE2_DIV8  (6<<11)
#define RCC_PPRE2_DIV16 (7<<11)

#define RCC_PPRE1_DIV2  (4<<8)
#define RCC_PPRE1_DIV4  (5<<8)
#define RCC_PPRE1_DIV8  (6<<8)
#define RCC_PPRE1_DIV16 (7<<8)

#define RCC_PLLSRC_HSI  (1<<15)

#define RCC_SW_PLL      (1<<1)
#define RCC_SWS_PLL     (1<<3)

#define DMA1_EN     (1<<0)
#define DMA2_EN     (1<<1)

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

typedef struct{
    uint32_t SYSCLK;
    uint32_t AHB;
    uint32_t APB1;
    uint32_t APB2;
} SYSCLK_t;

#define RCC ((RCC_TypeDef *) 0x40021000)

// only supports one timer 
void TimerEnable(uint8_t timer);

void set_clock_64Mhz(void);

#endif
