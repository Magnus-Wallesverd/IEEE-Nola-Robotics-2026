// system config, comparator, and opamp header
//

#ifndef SYSCFG_H
#define SYSCFG_H

#include <stdint.h>

typedef struct {
    volatile uint32_t CFGR1;     //0x00
    volatile uint32_t RCR;       //0x04
    volatile uint32_t EXTICR1;   //0x08
    volatile uint32_t EXTICR2;   //0x0C
    volatile uint32_t EXTICR3;   //0x10
    volatile uint32_t EXTICR4;   //0x14
    volatile uint32_t CFGR2;     //0x18
    volatile uint32_t RESERVED0; //0x1C
    volatile uint32_t RESERVED1; //0x20
    volatile uint32_t RESERVED2; //0x24
    volatile uint32_t RESERVED3; //0x28
    volatile uint32_t RESERVED4; //0x2C
    volatile uint32_t RESERVED5; //0x30
    volatile uint32_t RESERVED6; //0x34
    volatile uint32_t RESERVED7; //0x38
    volatile uint32_t RESERVED8; //0x3C
    volatile uint32_t RESERVED9; //0x40
    volatile uint32_t RESERVED10;//0x44
    volatile uint32_t CFGR4;     //0x48
}Syscfg_TypeDef;

#define SYSCFG ((Syscfg_Typedef *) 0x40010000)

#endif
