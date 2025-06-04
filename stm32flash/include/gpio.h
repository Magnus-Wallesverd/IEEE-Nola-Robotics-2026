/*
 * sim file
 *
*/

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stdio.h>

typedef struct{
    volatile uint32_t MODER;     
    volatile uint32_t OTYPER;   
    volatile uint32_t OSPEEDR;  
    volatile uint32_t PUPDR;    
    volatile uint32_t IDR;      
    volatile uint32_t ODR;      
    volatile uint32_t BSRR;     
    volatile uint32_t LCKR;     
    volatile uint32_t AFRL;     
    volatile uint32_t AFRH;     
    volatile uint32_t BRR;      
} GPIO_TypeDef;

extern GPIO_TypeDef gpioa;
extern GPIO_TypeDef gpiob;
extern GPIO_TypeDef gpioc;
extern GPIO_TypeDef gpiod;
extern GPIO_TypeDef gpioe;
extern GPIO_TypeDef gpiof;
extern GPIO_TypeDef gpiog;
extern GPIO_TypeDef gpioh;

// instance name of struct 
#define GPIOA (&gpioa)
#define GPIOB (&gpiob)
#define GPIOC (&gpioc)
#define GPIOD (&gpiod)
#define GPIOE (&gpioe)
#define GPIOF (&gpiof)
#define GPIOG (&gpiog)
#define GPIOH (&gpioh)

// prototype functions

// selects Input mode. takes GPIO struct and pins to set 
void SetPinInput(GPIO_TypeDef *port, uint32_t pins);

// selects Output mode. takes GPIO struct and pins to set 
void SetPinOutput(GPIO_TypeDef *port, uint32_t pins);

// selects Alternate mode. takes GPIO struct and pins to set 
void SetPinAlternate(GPIO_TypeDef *port, uint32_t pins);

// selects Analog mode. takes GPIO struct and pins to set 
void SetPinAnalog(GPIO_TypeDef *port, uint32_t pins);

// set pins in pull up mode
void SetPinPU(GPIO_TypeDef *port, uint32_t pins);

// set pins in pull down mode
void SetPinPD(GPIO_TypeDef *port, uint32_t pins);

// disables pullup pull down mode
void DisablePUPD(GPIO_TypeDef *port, uint32_t pins);

// Reads input pins (IDR)
uint32_t PinRead(GPIO_TypeDef *port);

// sets output values to desired pin(s) (BSRRL)
void PinWrite(GPIO_TypeDef *port, uint32_t pins);

// reset gpio pins with logic high to the desired pin(s) (BSRRH)
void ResetPins(GPIO_TypeDef *port, uint32_t pins);

void WritePin(GPIO_TypeDef *port, uint32_t pins); 

// sets alternate function
void AlternateFunctionSet(GPIO_TypeDef *port, uint32_t pins, uint32_t function);
#endif
