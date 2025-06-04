/* abstraction for gpio */

/*
 * TODO look up alternate functions
 * TODO look up how to read IDR
 *
 */

#include "gpio.h"

// mask to clear upper 16 bits since there are max 16 pins
const uint16_t masklow = 0xFFFF;

// these are for 2 bit mode settings
const uint8_t mode0  = 0b0000;     // input                 
const uint8_t mode1  = 0b0001;     // output                
const uint8_t mode2  = 0b0010;     // Alternate function    
const uint8_t mode3  = 0b0011;     // Analog                

// function to clear upper bits
static uint32_t ClearUpperBits(uint32_t input){
    return (input & masklow);
}

// function to clear lower bits
static uint32_t ClearLowerBits(uint32_t input){
    return (input << 16);
}

// takes reg (regster) to set, the pins, and the mode 
static uint32_t Generate2BitPinMask(uint32_t reg, uint32_t pins, uint8_t mode){
    
    uint32_t mask = 0;
    uint32_t clearmask = 0b11;
    uint32_t result = 0;

    // checks each bit for 1 then generates the bit mask
    for(int i = 0; i < 16; i++){
        if((pins >> i) & 1){
            result |= (clearmask << (2*i));
            mask |= (mode << (2*i));
        }
    }

    return ((reg & ~result)|mask);
}
 
static uint32_t Generate4BitPinMask(uint32_t reg, uint32_t pins, uint8_t mode){
    
    uint32_t mask = 0;
    uint32_t clearmask = 0b1111;
    uint32_t result = 0;

    // checks each bit for 1 then generates the bit mask
    for(int i = 0; i < 8; i++){
        if((pins >> i) & 1){
            result |= (clearmask << (4*i));
            mask |= (mode << (4*i));
        }
    }

    return ((reg & ~result)|mask);
}

//selects Input mode. takes GPIO struct and pins to set
void SetPinInput(GPIO_TypeDef *port, uint32_t pins){
    
    // clears upper 16 bits
    pins = ClearUpperBits(pins);
    
    // sets MODER
    port->MODER = Generate2BitPinMask(port->MODER, pins, mode0);
}

// selects General Output mode. takes GPIO struct and pins to set
void SetPinOutput(GPIO_TypeDef *port, uint32_t pins){
    
    // clears upper 16 bits
    pins = ClearUpperBits(pins);
    
    //sets MODER
    port->MODER = Generate2BitPinMask(port->MODER, pins, mode1);
}

// selects Alternate mode. takes GPIO struct and pins to set 
void SetPinAlternate(GPIO_TypeDef *port, uint32_t pins){
    
    // clears upper 16 bits
    pins = ClearUpperBits(pins);
    
    //sets MODER
    port->MODER = Generate2BitPinMask(port->MODER, pins, mode2);
}

// selects Analog mode. takes GPIO struct and pins to set 
void SetPinAnalog(GPIO_TypeDef *port, uint32_t pins){

    
    port->MODER = Generate2BitPinMask(port->MODER, pins, mode3);

}

// set pins in pull up mode
void SetPinPU(GPIO_TypeDef *port, uint32_t pins){
    
    pins = ClearUpperBits(pins);

    port->PUPDR = Generate2BitPinMask(port->PUPDR, pins, mode1);
}

// set pins in pull down mode
void SetPinPD(GPIO_TypeDef *port, uint32_t pins){

    pins = ClearUpperBits(pins);

    port->PUPDR = Generate2BitPinMask(port->PUPDR, pins, mode2);
}

// disables pullup pull down mode
void DisablePUPD(GPIO_TypeDef *port, uint32_t pins){
    
    pins = ClearUpperBits(pins);
    
    port->PUPDR = Generate2BitPinMask(port->PUPDR, pins, mode0);
}

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

// TBD
void WritePin(GPIO_TypeDef *port, uint32_t pins){
}

//sets an alternate function to the desired pin
//one function can be set to many pins
//calls generate4bitpinmask function above
void AlternateFunctionSet(GPIO_TypeDef *port, uint32_t pins, uint32_t function){
    
    pins = ClearUpperBits(pins);

    // sends upper bits first then lower bits
    port->AFRH = Generate4BitPinMask(port->AFRH, (pins >> 8), function);
    port->AFRL = Generate4BitPinMask(port->AFRL, (pins & 0x00FF), function);
}
