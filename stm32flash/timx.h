// defining clock register ports
// addresses are offset in bytes in the manual
// TODO fix the general purpose timer structs
// timer 15 16 17 are mapped different
// current struct supports timer 15
#ifndef TIMX_H
#define TIMX_H

#include <stdint.h>

// Advanced Timers: 1, 8, 20
typedef struct{
volatile uint32_t CR1;      //0x00
volatile uint32_t CR2;      //0x04 
volatile uint32_t SMCR;     //0x08
volatile uint32_t DIER;     //0x0C
volatile uint32_t SR;       //0x10
volatile uint32_t EGR;      //0x14
volatile uint32_t CCMR1;    //0x18
volatile uint32_t CCMR2;    //0x1C
volatile uint32_t CCER;     //0x20
volatile uint32_t CNT;      //0x24
volatile uint32_t PSC;      //0x28
volatile uint32_t ARR;      //0x2C
volatile uint32_t RCR;      //0x30
volatile uint32_t CCR1;     //0x34
volatile uint32_t CCR2;     //0x38
volatile uint32_t CCR3;     //0x3C
volatile uint32_t CCR4;     //0x40
volatile uint32_t BDTR;     //0x44
volatile uint32_t DCR;      //0x48
volatile uint32_t DMAR;     //0x4C
volatile uint32_t OR;       //0x50
volatile uint32_t CCMR3;    //0x54
volatile uint32_t CCR5;     //0x58
volatile uint32_t CCR6;     //0x5C
} Adv_TIM_TypeDef;


// General Timers
typedef struct{
    volatile uint32_t CR1;      //0x00
    volatile uint32_t CR2;      //0x04
    volatile uint32_t SMCR;     //0x08
    volatile uint32_t DIER;     //0x0C
    volatile uint32_t SR;       //0x10
    volatile uint32_t EGR;      //0x14
    volatile uint32_t CCMR1;    //0x18
    volatile uint32_t RESERVE1; //0x1C
    volatile uint32_t CCER;     //0x20
    volatile uint32_t CNT;      //0x24
    volatile uint32_t PSC;      //0x28
    volatile uint32_t ARR;      //0x2C
    volatile uint32_t RCR;      //0x30
    volatile uint32_t CCR1;     //0x34
    volatile uint32_t CCR2;     //0x38
    volatile uint32_t RESERVE2; //0x3C
    volatile uint32_t RESERVE3; //0x40
    volatile uint32_t BDTR;     //0x44
    volatile uint32_t DCR;      //0x48
    volatile uint32_t DMAR;     //0x4C
} Gen_TIM_TypeDef;


// Basic Timers
typedef struct{
   volatile uint32_t CR1;      //0x00
   volatile uint32_t CR2;      //0x04
   volatile uint32_t RESERVE1; //0x08
   volatile uint32_t DIER;     //0x0C
   volatile uint32_t SR;       //0x10
   volatile uint32_t EGR;      //0x14
   volatile uint32_t RESERVE2; //0x18
   volatile uint32_t RESERVE3; //0x1C
   volatile uint32_t RESERVE4; //0x20
   volatile uint32_t CNT;      //0x24
   volatile uint32_t PSC;      //0x28
   volatile uint32_t ARR;      //0x2C
   volatile uint32_t RESERVE5; //0x30
   volatile uint32_t RESERVE6; //0x34
   volatile uint32_t RESERVE7; //0x38
   volatile uint32_t RESERVE8; //0x3C
   volatile uint32_t RESERVE9; //0x40
   volatile uint32_t RESERVE10;//0x44
   volatile uint32_t RESERVE11;//0x48
   volatile uint32_t RESERVE12;//0x4C
   volatile uint32_t RESERVE13;//0x50
} Basic_TIM_TypeDef;


// instance name of struct ((reference struct pointer *) desired address)
#define TIM1  ((Adv_TIM_TypeDef *) 0x40012C00)
#define TIM8  ((Adv_TIM_TypeDef *) 0x40013400)
#define TIM20 ((Adv_TIM_TypeDef *) 0x40015000)

#define TIM2  ((Gen_TIM_TypeDef *) 0x40000000)
#define TIM3  ((Gen_TIM_TypeDef *) 0x40000400)
#define TIM4  ((Gen_TIM_TypeDef *) 0x40000800)
#define TIM15 ((Gen_TIM_TypeDef *) 0x40014000)
//#define TIM16 ((Gen_TIM_TypeDef *) 0x40014400)
//#define TIM17 ((Gen_TIM_TypeDef *) 0x40014800)

#define TIM6  ((Basic_TIM_TypeDef *) 0x40001000)
#define TIM7  ((Basic_TIM_TypeDef *) 0x40001400)

// enable timer 1, 8 or 20 with some loaded preset
// only one timer per call
void InitAdvTIM(Adv_TIM_TypeDef *port);

// turns on pwm mode 
void ActivatePWM(Adv_TIM_TypeDef *port);

// enable timer 2, 3, or 4
// one timer per call
void InitGenTIM(Gen_TIM_TypeDef * port);

// enables timer 6 & 7
// one timer per call
void InitBasicTIM(Basic_TIM_TypeDef *port);


#endif
