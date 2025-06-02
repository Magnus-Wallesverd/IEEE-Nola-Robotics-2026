/* abstraction for gpio */

/*
 * TODO look up alternate functions
 * TODO look up how to read IDR
 *
 */

#include "gpio.h"

// mask to clear upper 16 bits since there are max 16 pins
const uint32_t initmasklow = 0xFFFF;
const uint32_t initmaskhigh = 0xFFFF0000;

// function to clear upper bits
uint32_t ClearUpperBits(uint32_t pins){
    return (pins & initmasklow);
}

// function to clear lower bits
uint32_t ClearLowerBits(uint32_t pins){
    return (pins <<16);
}

/* 
 * selects Input mode. takes GPIO struct and pins to set
 * only sets pins passed into function
 * USER should only input a uint16 of size uint32
 * works but check
 */
void SetPinInput(GPIO_TypeDef *port, uint32_t pins){
    
    // clears upper 16 bits
    pins = ClearUpperBits(pins);

    // used to create a bit mask for MODER
    uint32_t mask = 0b11;
    uint32_t result = 0;
    
    // checks each bit for a 1 then generates the bit mask
    for(char i = 0 ; i < 16; i++){
        if((pins >> i) & 0x01){
            result |= (mask << (2*i));
        }
    }
    
    port->MODER &= ~result;
}

/* 
 * selects General Output mode. 
 * takes GPIO struct and pins to set
 * only sets pins passed into function
 * USER should only input a uint16 of size uint32
 * works
 */
void SetPinOutput(GPIO_TypeDef *port, uint32_t pins){
    
    // clears upper 16 bits
    pins = ClearUpperBits(pins);

    // used to create bit mask for MODER
    uint32_t mask = 0b10;
    uint32_t result = 0;

    // checks each bit for 1 then generates the bit mask
    for(int i = 0; i < 16; i++){
        if((pins >> i) & 1){
            result |= (mask << (2*i));
        }
    }
    
    port->MODER &= ~result;
}

// selects Alternate mode. takes GPIO struct and pins to set 
void SetPinAlternate(GPIO_TypeDef *port, uint32_t pins){
}

// selects Analog mode. takes GPIO struct and pins to set 
void SetPinAnalog(GPIO_TypeDef *port, uint32_t pins){}

// set pins in pull up mode
void SetPinPU(GPIO_TypeDef *port, uint32_t pins){}

// set pins in pull down mode
void SetPinPD(GPIO_TypeDef *port, uint32_t pins){}

// disables pullup pull down mode
void DisablePUPD(GPIO_TypeDef *port, uint32_t pins){}

// Reads input pins (IDR)
uint32_t PinRead(GPIO_TypeDef *port){return 0;}

// sets output values to desired pin(s) (BSRRL)
void PinWrite(GPIO_TypeDef *port, uint32_t pins){
    pins = ClearUpperBits(pins);
    port->BSRR = pins;
}

// reset gpio pins with logic high to the desired pin(s) (BSRRH)
void ResetPins(GPIO_TypeDef *port, uint32_t pins){
    pins = ClearLowerBits(pins);
    port->BSRR = pins;
}

void WritePin(GPIO_TypeDef *port, uint32_t pins){
}

//sets an alternate function to the desired pin
void AlternateFunctionSet(GPIO_TypeDef *port, uint32_t function, uint32_t pins){}
