/* abstraction for gpio */

/*
 * TODO take a pointer to an array of pins for multiple pin sets
 * TODO setup up a bit mask for any 32 bit registers using 16 bit inputs
 * TODO look up alternate functions
 */

#include "gpio.h"


// selects Input mode. takes GPIO struct and pins to set
// only sets pins input to function
// USER should only input a uint16 of size uint32
void SetPinInput(GPIO_TypeDef *port, uint32_t pins){
    
    // used to create a bit mask for MODER
    uint32_t initmask = 0xFFFF
    uint32_t mask = 0b11;
    uint32_t result = 0;
    
    // clears upper 16 bits in case of user error
    pins &= initmask;
    
    // checks each bit for a 1 then generates the bit mask
    for(char i = 0 ; i < 16; i++){
        if((pins >> i) & 0x01){
            result |= (mask << (2*i));
        }
    }
    
    port->MODER &= ~result;
}

// selects Output mode. takes GPIO struct and pins to set 
void SetPinOutput(GPIO_TypeDef *port, uint32_t pins){}

// selects Alternate mode. takes GPIO struct and pins to set 
void SetPinAlternate(GPIO_TypeDef *port, uint32_t pins){}

// selects Analog mode. takes GPIO struct and pins to set 
void SetPinAnalog(GPIO_TypeDef *port, uint32_t pins){}

// set pins in pull up mode
void SetPinPU(GPIO_TypeDef *port, uint32_t pins){}

// set pins in pull down mode
void SetPinPD(GPIO_TypeDef *port, uint32_t pins){}

// disables pullup pull down mode
void DisablePUPD(GPIO_TypeDef *port, uint32_t pins){}

// Reads input pins (IDR)
uint32_t ReadInput(GPIO_TypeDef *port){}

// sets output values to desired pin(s) (BSRRL)
void SetOutputPins(GPIO_TypeDef *port, uint32_t pins){}

// reset gpio pins with logic high to the desired pin(s) (BSRRH)
void ResetPins(uint32_t pins){}

//sets an alternate function to the desired pin
void AlternateFunctionSet(GPIO_TypeDef *port, uint32_t function, uint32_t pins){}
