// defining the register ports
// addresses are offset in bytes in the manual

#ifndef PORTDEF_H
#define PORTDEF_H

#include <stdint.h>

// implementing a struct based system where 32bit variables are declared
// then a pointer will just fill up the whole block with struct variables
// much small scale than copy paste

typedef struct{
volatile uint32_t MODER;    //x00 
volatile uint32_t OTYPER;   //x04
volatile uint32_t OSPEEDR;  //x08
volatile uint32_t PUPDR;    //x0C
volatile uint32_t IDR;      //x10
volatile uint32_t ODR;      //x14
volatile uint32_t BSRR;     //x18
volatile uint32_t LCKR;     //x1C
volatile uint32_t AFRL;     //x20
volatile uint32_t AFRH;     //x24
volatile uint32_t BRR;      //x28
} GPIO_TypeDef;


// instance name of struct ((reference struct pointer *) desired address)
#define GPIOA ((GPIO_TypeDef *) 0x48000000)
#define GPIOB ((GPIO_TypeDef *) 0x48000400)
#define GPIOC ((GPIO_TypeDef *) 0x48000800)
#define GPIOD ((GPIO_TypeDef *) 0x48000C00)
#define GPIOE ((GPIO_TypeDef *) 0x48001000)
#define GPIOF ((GPIO_TypeDef *) 0x48001400)
#define GPIOG ((GPIO_TypeDef *) 0x48001800)
#define GPIOH ((GPIO_TypeDef *) 0x48001C00)

#endif
