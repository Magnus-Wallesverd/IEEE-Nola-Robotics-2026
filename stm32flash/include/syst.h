#ifndef SYST_H
#define SYST_H

#include <stdint.h>

typedef struct {
    volatile uint32_t CSR;     //0x00
    volatile uint32_t RVR;     //0x04
    volatile uint32_t CVR;     //0x08
    volatile uint32_t CALIB;   //0x1C
}Syst_TypeDef;

#define SYST ((Syst_TypeDef *) 0xE000E010)

#endif
