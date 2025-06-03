// defining the register ports
// addresses are offset in bytes in the manual
/*
 * TODO abstract all register if necessary
*/


#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

// implementing a struct based system where 32bit variables are declared
// then a pointer will just fill up the whole block with struct variables

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

//sets an alternate function to the desired pin
void AlternateFunctionSet(GPIO_TypeDef *port, uint32_t function, uint32_t pins);

void WritePin(GPIO_TypeDef *port, uint32_t pins); 
#endif
