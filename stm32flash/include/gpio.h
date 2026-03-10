#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

#define PA0  1<<0
#define PA1  1<<1
#define PA2  1<<2
#define PA3  1<<3
#define PA4  1<<4
#define PA5  1<<5
#define PA6  1<<6
#define PA7  1<<7
#define PA8  1<<8
#define PA9  1<<9
#define PA10 1<<10
#define PA11 1<<11
#define PA12 1<<12
#define PA13 1<<13
#define PA14 1<<14
#define PA15 1<<15

#define PB0  1<<0
#define PB1  1<<1
#define PB2  1<<2
#define PB3  1<<3
#define PB4  1<<4
#define PB5  1<<5
#define PB6  1<<6
#define PB7  1<<7
#define PB8  1<<8
#define PB9  1<<9
#define PB10 1<<10
#define PB11 1<<11
#define PB12 1<<12
#define PB13 1<<13
#define PB14 1<<14
#define PB15 1<<15

#define PC0  1<<0
#define PC1  1<<1
#define PC2  1<<2
#define PC3  1<<3
#define PC4  1<<4
#define PC5  1<<5
#define PC6  1<<6
#define PC7  1<<7
#define PC8  1<<8
#define PC9  1<<9
#define PC10 1<<10
#define PC11 1<<11
#define PC12 1<<12
#define PC13 1<<13
#define PC14 1<<14
#define PC15 1<<15

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

// set output type. takes GPIO struct and pins to set
void SetOutputType(GPIO_TypeDef *port, uint32_t pins, uint8_t bit);

// set output speed. takes GPIO struct and pins to set
void SetOutputSpeed(GPIO_TypeDef *port, uint32_t pins, uint8_t bits);

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

// dummy function
void blink_led(void*);

// turns camera on
void camera_setup(void);

#endif
