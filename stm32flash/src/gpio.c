/* abstraction for gpio */

/*
 * TODO look up alternate functions
 * TODO look up how to read IDR
 *
 */

#include "gpio.h"

//aggregate initialization
GPIO_TypeDef gpioa = {0};
GPIO_TypeDef gpiob = {0};
GPIO_TypeDef gpioc = {0};
GPIO_TypeDef gpiod = {0};
GPIO_TypeDef gpioe = {0};
GPIO_TypeDef gpiof = {0};
GPIO_TypeDef gpiog = {0};
GPIO_TypeDef gpioh = {0};

// mask to clear upper 16 bits since there are max 16 pins
const uint32_t masklow = 0xFFFF;

// these are for 2 bit mode settings
const uint8_t mode0  = 0b00;     // input                 
const uint8_t mode1  = 0b01;     // output                
const uint8_t mode2  = 0b10;     // Alternate function    
const uint8_t mode3  = 0b11;     // Analog                

static void UpdateODR(GPIO_TypeDef *port){
    port->ODR ^= port->BSRR;
    port->BSRR &= 0;
}


static void ClearODR(GPIO_TypeDef *port){
    port->ODR &= ~(port->BSRR >> 16);
    port->BSRR &= 0;
}

// function to clear upper bits
static uint32_t ClearUpperBits(uint32_t input){
    return (input & masklow);
}

// function to clear lower bits
static uint32_t ClearLowerBits(uint32_t input){
    return (input << 16);
}


// takes reg (regster) to set, the pins, bitwidth for clearing mask, and the mode 
static uint32_t GenerateBitMask(uint32_t reg, uint32_t pins, uint8_t bitwidth, uint8_t mode){
    
    uint32_t mask = 0;
    uint32_t clearmask;
    uint32_t result = 0;

    if(bitwidth == 2){
        clearmask = 0b11;
    }
    else
        clearmask = 0b1111;

    // checks each bit for 1 then generates the bit mask
    for(int i = 0; i < 32/bitwidth; i++){
        if((pins >> i) & 1){
            result |= (clearmask << (bitwidth*i));
            mask |= (mode << (bitwidth*i));
        }
    }

    return ((reg & ~result)|mask);
}

//selects Input mode. takes GPIO struct and pins to set
void SetPinInput(GPIO_TypeDef *port, uint32_t pins){
    
    // clears upper 16 bits
    pins = ClearUpperBits(pins);
    
    // sets MODER
    port->MODER = GenerateBitMask(port->MODER, pins, 2, mode0);
    printf("[Input] MODER = 0x%X Using GBM\n", port->MODER);
}

// selects General Output mode. takes GPIO struct and pins to set
void SetPinOutput(GPIO_TypeDef *port, uint32_t pins){
    
    // clears upper 16 bits
    pins = ClearUpperBits(pins);
    
    //sets MODER
    port->MODER = GenerateBitMask(port->MODER, pins, 2, mode1);
    printf("[Output] MODER = 0x%X Using GBM\n", port->MODER);

}

// selects Alternate mode. takes GPIO struct and pins to set 
void SetPinAlternate(GPIO_TypeDef *port, uint32_t pins){
    
    // clears upper 16 bits
    pins = ClearUpperBits(pins);
    
    //sets MODER
    port->MODER = GenerateBitMask(port->MODER, pins, 2, mode2);
    printf("[AltFunction] MODER = 0x%X Using GBM\n", port->MODER);
}

// selects Analog mode. takes GPIO struct and pins to set 
void SetPinAnalog(GPIO_TypeDef *port, uint32_t pins){

    port->MODER = GenerateBitMask(port->MODER, pins, 2, mode3);
    printf("[Analog] MODER = 0x%X Using GBM\n", port->MODER);
}


// set pins in pull up mode
void SetPinPU(GPIO_TypeDef *port, uint32_t pins){
    
    pins = ClearUpperBits(pins);

    port->PUPDR = GenerateBitMask(port->PUPDR, pins, 2, mode1);
    printf("[Pull-Up] PUPDR = 0x%X Using GBM\n", port->PUPDR);
}

// set pins in pull down mode
void SetPinPD(GPIO_TypeDef *port, uint32_t pins){

    pins = ClearUpperBits(pins);

    port->PUPDR = GenerateBitMask(port->PUPDR, pins, 2, mode2);
    printf("[Pull-Down] PUPDR = 0x%X Using GBM\n", port->PUPDR);
}

// disables pullup pull down mode
void DisablePUPD(GPIO_TypeDef *port, uint32_t pins){
    
    pins = ClearUpperBits(pins);
    
    port->PUPDR = GenerateBitMask(port->PUPDR, pins, 2, mode0);
    printf("[No Pull] PUPDR = 0x%X Using GBM\n", port->PUPDR);
}

// Reads input pins (IDR)
uint32_t PinRead(GPIO_TypeDef *port){
    // printf("[GPIO] IDR = 0x%X Using GBM\n", port->IDR);
    return 0;
}

// sets output values to desired pin(s) (BSRRL)
void PinWrite(GPIO_TypeDef *port, uint32_t pins){
    pins = ClearUpperBits(pins);
    port->BSRR = pins;
    UpdateODR(port);
    printf("[PinWrite] ODR = 0x%X Using GBM\n", port->ODR);
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
    port->AFRH = GenerateBitMask(port->AFRH, (pins >> 8), 4, function);
    port->AFRL = GenerateBitMask(port->AFRL, (pins & 0xFF), 4, function);

    printf("[Function] AFRH = 0x%X Using GBM\n", port->AFRH);
    printf("[Function] AFRL = 0x%X Using GBM\n", port->AFRL);
}
