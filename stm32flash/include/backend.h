#ifndef BACKEND_H
#define BACKEND_H

#include <stdint.h>
#include "stm32f303.h"

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



#define STK ((Systick_TypeDef *) 0xE000E010)
#define SCB ((SCB_TypeDef *)     0xE000ED00)

#endif // !BACKEND_H
