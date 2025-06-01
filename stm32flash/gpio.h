// defining the register ports
// addresses are offset in bytes in the manual

#ifndef GPIO_H
#define GPIO_H

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

// prototype functions

// parameters are pointer to struct, pin to set, and mode of pin
//void SetMode(GPIO_TypeDef *port, int pin, char mode);

// selects Input mode. takes GPIO struct and pins to set 
void SetPinInput(GPIO_TypeDef *port, char pins);

// selects Output mode. takes GPIO struct and pins to set 
void SetPinOutput(GPIO_TypeDef *port, char pins);

// selects Alternate mode. takes GPIO struct and pins to set 
void SetPinAlternate(GPIO_TypeDef *port, char pins);

// selects Analog mode. takes GPIO struct and pins to set 
void SetPinAnalog(GPIO_TypeDef *port, char pins);

// set pins in pull up mode
void SetPinPU(GPIO_TypeDef *port, char pins);

// set pins in pull down mode
void SetPinPD(GPIO_TypeDef *port, char pins);

// disables pullup pull down mode
void DisablePUPD(GPIO_TypeDef *port, char pins);

// reset gpio pins logic high (BRR)
void ResetPins(char pins);

// Reads input pins (IDR)
//void ReadInput();

// sets output values to desired pins
void SetOutputPins(GPIO_TypeDef *port, char pins) 


#endif
