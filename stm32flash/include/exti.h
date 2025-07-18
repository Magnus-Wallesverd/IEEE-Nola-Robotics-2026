// external interrupt registers 
// enables falling/rising trigger and bit masks
// rm0316 pg.301

#ifndef EXTI_H
#define EXTI_H

typedef struct{
    volatile uint32_t IMR1;     //0x00
    volatile uint32_t EMR1;     //0x04
    volatile uint32_t RTSR1;    //0x08
    volatile uint32_t FTSR1;    //0x0C
    volatile uint32_t SWIER1;   //0x10
    volatile uint32_t PR1;      //0x14
    volatile uint32_t RESERVED0;//0x18
    volatile uint32_t RESERVED1;//0x1C
    volatile uint32_t IMR2;     //0x20
    volatile uint32_t EMR2;     //0x24
    volatile uint32_t RTSR2;    //0x28
    volatile uint32_t FTSR2;    //0x2C
    volatile uint32_t SWIER2;   //0x30
    volatile uint32_t PR2;      //0x34
}Exti_TypeDef;

#define EXTI ((Exti_TypeDef *) 0x40010400)

#endif
