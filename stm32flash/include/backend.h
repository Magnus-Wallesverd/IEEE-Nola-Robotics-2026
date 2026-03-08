#ifndef BACKEND_H
#define BACKEND_H

#include <stdint.h>

#define NVIC_IRQ_PRIORITY0 0
#define NVIC_IRQ_PRIORITY1  (1 << 4)
#define NVIC_IRQ_PRIORITY2  (2 << 4)
#define NVIC_IRQ_PRIORITY3  (3 << 4)
#define NVIC_IRQ_PRIORITY4  (4 << 4)
#define NVIC_IRQ_PRIORITY5  (5 << 4)
#define NVIC_IRQ_PRIORITY6  (6 << 4)
#define NVIC_IRQ_PRIORITY7  (7 << 4)
#define NVIC_IRQ_PRIORITY8  (8 << 4)
#define NVIC_IRQ_PRIORITY9  (9 << 4)
#define NVIC_IRQ_PRIORITY10 (10 << 4)
#define NVIC_IRQ_PRIORITY11 (11 << 4)
#define NVIC_IRQ_PRIORITY12 (12 << 4)
#define NVIC_IRQ_PRIORITY13 (13 << 4)
#define NVIC_IRQ_PRIORITY14 (14 << 4)
#define NVIC_IRQ_PRIORITY15 (15 << 4)

#define NVIC_USART1 (1<<(37-32))
#define NVIC_USART2 (1<<(38-32))
#define NVIC_USART3 (1<<(39-32))
#define NVIC_UART4  (1<<(52-32))
#define NVIC_UART5  (1<<(53-32))

typedef struct{
    volatile uint32_t ISER0;
    volatile uint32_t ISER1;
    volatile uint32_t ISER2;
    volatile uint32_t ISER3;
    volatile uint32_t ISER4;
    volatile uint32_t ISER5;
    volatile uint32_t ISER6;
    volatile uint32_t ISER7;
    volatile uint32_t ICER0;
    volatile uint32_t ICER1;
    volatile uint32_t ICER2;
    volatile uint32_t ICER3;
    volatile uint32_t ICER4;
    volatile uint32_t ICER5;
    volatile uint32_t ICER6;
    volatile uint32_t ICER7;
    volatile uint32_t ISPR0;
    volatile uint32_t ISPR1;
    volatile uint32_t ISPR2;
    volatile uint32_t ISPR3;
    volatile uint32_t ISPR4;
    volatile uint32_t ISPR5;
    volatile uint32_t ISPR6;
    volatile uint32_t ISPR7;
    volatile uint32_t ICPR0;
    volatile uint32_t ICPR1;
    volatile uint32_t ICPR2;
    volatile uint32_t ICPR3;
    volatile uint32_t ICPR4;
    volatile uint32_t ICPR5;
    volatile uint32_t ICPR6;
    volatile uint32_t ICPR7;
    volatile uint32_t IABR0;
    volatile uint32_t IABR1;
    volatile uint32_t IABR2;
    volatile uint32_t IABR3;
    volatile uint32_t IABR4;
    volatile uint32_t IABR5;
    volatile uint32_t IABR6;
    volatile uint32_t IABR7;
    volatile uint32_t STIR;
}NVIC_TypeDef;

typedef struct{
    volatile uint32_t IPR0;
    volatile uint32_t IPR1;
    volatile uint32_t IPR2;
    volatile uint32_t IPR3;
    volatile uint32_t IPR4;
    volatile uint32_t IPR5;
    volatile uint32_t IPR6;
    volatile uint32_t IPR7;
    volatile uint32_t IPR8;
    volatile uint32_t IPR9;
    volatile uint32_t IPR10;
    volatile uint32_t IPR11;
    volatile uint32_t IPR12;
    volatile uint32_t IPR13;
    volatile uint32_t IPR14;
    volatile uint32_t IPR15;
    volatile uint32_t IPR16;
    volatile uint32_t IPR17;
    volatile uint32_t IPR18;
    volatile uint32_t IPR19;
    volatile uint32_t IPR20;
    volatile uint32_t IPR21;
    volatile uint32_t IPR22;
    volatile uint32_t IPR23;
    volatile uint32_t IPR24;
    volatile uint32_t IPR25;
    volatile uint32_t IPR26;
    volatile uint32_t IPR27;
    volatile uint32_t IPR28;
    volatile uint32_t IPR29;
    volatile uint32_t IPR30;
    volatile uint32_t IPR31;
    volatile uint32_t IPR32;
    volatile uint32_t IPR33;
    volatile uint32_t IPR34;
    volatile uint32_t IPR35;
    volatile uint32_t IPR36;
    volatile uint32_t IPR37;
    volatile uint32_t IPR38;
    volatile uint32_t IPR39;
    volatile uint32_t IPR40;
    volatile uint32_t IPR41;
    volatile uint32_t IPR42;
    volatile uint32_t IPR43;
    volatile uint32_t IPR44;
    volatile uint32_t IPR45;
    volatile uint32_t IPR46;
    volatile uint32_t IPR47;
    volatile uint32_t IPR48;
    volatile uint32_t IPR49;
    volatile uint32_t IPR50;
    volatile uint32_t IPR51;
    volatile uint32_t IPR52;
    volatile uint32_t IPR53;
    volatile uint32_t IPR54;
    volatile uint32_t IPR55;
    volatile uint32_t IPR56;
    volatile uint32_t IPR57;
    volatile uint32_t IPR58;
    volatile uint32_t IPR59;
} NVIC_IPR_TypeDef;

typedef struct{
    volatile uint32_t CPUID;    //0x00
    volatile uint32_t ICSR;     //0x04
    volatile uint32_t VTOR;     //0x08
    volatile uint32_t AIRCR;    //0x0C
    volatile uint32_t SCR;      //0x10
    volatile uint32_t CCR;      //0x14
    volatile uint32_t SHPR1;    //0x18
    volatile uint32_t SHPR2;    //0x1C
    volatile uint32_t SHPR3;    //0x20
    volatile uint32_t SHCSR;    //0x24
    volatile uint32_t CFSR;     //0x28
    volatile uint32_t HFSR;     //0x2C
    volatile uint32_t MMAR;     //0x30
    volatile uint32_t BFAR;     //0x34
    volatile uint32_t AFSR;     //0x38
}SCB_TypeDef;

typedef struct {
    volatile uint32_t CTRL;      //0x00
    volatile uint32_t LOAD;      //0x04
    volatile uint32_t VAL;       //0x08
    volatile uint32_t CALIB;     //0x0C
}Systick_TypeDef;

typedef struct {
    volatile uint32_t SHPR3;
}SHPR3_TypeDef;

#define NVIC        ((NVIC_TypeDef *)    0xE000E100)
#define NVIC_IPR    ((NVIC_IPR_TypeDef *)    0xE000E400)
#define STK         ((Systick_TypeDef *) 0xE000E010)
#define SCB         ((SCB_TypeDef *)     0xE000ED00)

#endif // !BACKEND_H
