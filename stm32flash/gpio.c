/* abstraction for gpio */

/*
 * TODO take a pointer to an array of pins for multiple pin sets
 * TODO setup up a bit mask for any 32 bit registers using 16 bit inputs
 * TODO look up alternate functions
 */

#include "gpio.h"


// selects Input mode. takes GPIO struct and pins to set 
void SetPinInput(GPIO_TypeDef *port, uint32_t pins){
    
    //i need to find a way to shift the pin 2*(0->8) pins
    //to bit shift individual bits i need to mask them
    
    unsigned char mode = 0b00;
    /*
    for(char i = 16 ; i > 0; i--){
        if((pins >> i) & 0x01){
            pins ^= (mode<<) 
        }
    }
    
}
*/

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
