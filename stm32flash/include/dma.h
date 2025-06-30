#ifndef DMA_H
#define DMA_H

#include <stdint.h>

typedef struct{
    volatile uint32_t ISR;       //0x00
    volatile uint32_t IFCR;     //0x04
    volatile uint32_t CCR1;      //0x08
    volatile uint32_t CNDTR1; //0X0C
    volatile uint32_t CPAR1; //0x10
    volatile uint32_t CMAR1;   //0x14
    volatile uint32_t Reserved1;  //0x18
    volatile uint32_t CCR2;  //0x1C
    volatile uint32_t CNDTR2;     //0x20
    volatile uint32_t CPAR2;      //0x24
    volatile uint32_t CMAR2;  //0x28
    volatile uint32_t Reserved2;    //0x2C
    volatile uint32_t CCR3;    //0x30
    volatile uint32_t CNDTR3;    //0x34
    volatile uint32_t CPAR3;    //0x38
    volatile uint32_t CMAR3;    //0x3C
    volatile uint32_t Reserved3;    //0x40

    volatile uint32_t CCR4;    //0x44
    volatile uint32_t CNDTR4;    //0x48
    volatile uint32_t CPAR4;    //0x4C
    volatile uint32_t CMAR4;    //0x50
    volatile uint32_t Reserved4;    //0x54
	
    volatile uint32_t CCR5;    //0x58
    volatile uint32_t CNDTR5;    //0x5C
    volatile uint32_t CPAR5;    //0x60
    volatile uint32_t CMAR5;    //0x64
    volatile uint32_t Reserved5;    //0x68
	
    volatile uint32_t CCR6;    //0x6C
    volatile uint32_t CNDTR6;    //0x70
    volatile uint32_t CPAR6;    //0x74
    volatile uint32_t CMAR6;    //0x78
    volatile uint32_t Reserved6;    //0x7C
				    //
    volatile uint32_t CCR7;    //0x80
    volatile uint32_t CNDTR7;    //0x84
    volatile uint32_t CPAR7;    //0x88
    volatile uint32_t CMAR7;    //0x8C

} DMA_TypeDef;

#define DMA ((DMA_TypeDef *) 0x40020000)
#define DMA2 ((DMA2_TypeDef *) 0x40020400)
 
#endif
