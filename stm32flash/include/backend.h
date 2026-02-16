#ifndef BACKEND_H
#define BACKEND_H

#include <stdint.h>

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

#define NVIC  ((NVIC_TypeDef *)    0xE000E100)
#define STK   ((Systick_TypeDef *) 0xE000E010)
#define SCB   ((SCB_TypeDef *)     0xE000ED00)

#endif // !BACKEND_H
