#include "i2c.h"
#include "rcc.h"
#include "gpio.h"
#include <stdint.h>

// Initialize the I2C peripheral with basic settings
void I2C_Init(I2C_TypeDef *I2Cx, uint32_t timing) {
    RCC->APB1ENR |= (1 << 21);
    // Configure timing
    I2Cx->TIMINGR = timing;
    // Enable peripheral
    I2Cx->CR1 |= (1 << 0);  // PE = 1
}

// Send a start condition and address a device for writing
void I2C_Write(I2C_TypeDef *I2Cx, uint8_t dev_addr, const uint8_t *data, uint16_t size) {
    
    // 1. Set up transfer: address, size, write mode, start
    // SADD[0] SADD[8] SADD[9] are dont care x.
    I2Cx->CR2 = (dev_addr << 1) | (size << 16); // SADD[7:1], NBYTES[23:16]
    I2Cx->CR2 &= ~(1 << 10); // RD_WRN = 0 (write)
    I2Cx->CR2 |= (1 << 13);  // START

    for (uint16_t i = 0; i < size; i++) {
        // Wait until TXIS (Transmit interrupt status) is set
        while (!(I2Cx->ISR & (1 << 1)));
        // Write data
        I2Cx->TXDR = data[i];
    }
    // Wait until TC (Transfer Complete) is set
    while (!(I2Cx->ISR & (1 << 6)));
    // Send STOP
    I2Cx->CR2 |= (1 << 14);
    // Wait until STOPF (Stop detection flag) is set
    while (!(I2Cx->ISR & (1 << 5)));
    // Clear STOPF by writing to ICR
    I2Cx->ICR |= (1 << 5);
}

// Send a start condition and address a device for reading
void I2C_Read(I2C_TypeDef *I2Cx, uint8_t dev_addr, uint8_t *data, uint16_t size) {

    // 1. Set up transfer: address, size, read mode, start
    I2Cx->CR2 = (dev_addr << 1) | (size << 16);
    I2Cx->CR2 |= (1 << 10); // RD_WRN = 1 (read)
    I2Cx->CR2 |= (1 << 13); // START

    for (uint16_t i = 0; i < size; i++) {
        // Wait until RXNE (Receive buffer not empty)
        while (!(I2Cx->ISR & (1 << 2)));
        // Read data
        data[i] = (uint8_t)(I2Cx->RXDR & 0xFF);
    }

    // Wait until TC (Transfer Complete) is set
    while (!(I2Cx->ISR & (1 << 6)));
    // Send STOP
    I2Cx->CR2 |= (1 << 14);
    // Wait until STOPF (Stop detection flag) is set
    while (!(I2Cx->ISR & (1 << 5)));
    // Clear STOPF by writing to ICR
    I2Cx->ICR |= (1 << 5);
}

// Write a single byte to a device register
void I2C_WriteReg(I2C_TypeDef *I2Cx, uint8_t dev_addr, uint8_t reg_addr, uint8_t value) {

    // Send the register address, then the data byte(s)
    buf[0] = reg_addr;
    buf[1] = value;
    I2C_Write(I2Cx, dev_addr, buf, 2);
}

// Read a single byte from a device register
void I2C_ReadReg(I2C_TypeDef *I2Cx, uint8_t dev_addr, uint8_t reg_addr, uint8_t *value) {

    // Write the register address (no STOP)
    I2Cx->CR2 = (dev_addr << 1) | (1 << 16); // NBYTES = 1
    I2Cx->CR2 &= ~(1 << 10); // Write mode
    I2Cx->CR2 |= (1 << 13); // START

    while(!(I2Cx->ISR & (1 << 1))); // Wait TXIS
    I2Cx->TXDR = reg_addr;
    while(!(I2Cx->ISR & (1 << 6))); // Wait TC (not STOP)
                                        
    // Issue a repeated START to read data
    I2Cx->CR2 = (dev_addr << 1) | (1 << 10) | (1 << 13) | (1 << 16); // Read mode, NBYTES=1, START

    while(!(I2Cx->ISR & (1 << 2)));   // Wait RXNE
    *value = (uint8_t)(I2Cx->RXDR & 0xFF);

    while(!(I2Cx->ISR & (1 << 6)));
    I2Cx->CR2 |= (1 << 14); // STOP
    while(!(I2Cx->ISR & (1 << 5)));
    I2Cx->ICR |= (1 << 5);
}

void I2C_WriteReg_Wrapper(void* args){
    (void) args;
    I2C_WriteReg(I2C1, BNO055, 0x10, 0xFF);
}

void start_condition(void){
    for(int i = 0; i < 10000;i++);
    GPIOA->ODR = (1 << 9)|(1<<10); // both hi
    for(int i = 0; i < 20000;i++);
    GPIOA->ODR ^= (1 << 10); // pull SDA low
    for(int i = 0; i < 20000;i++);
    GPIOA->ODR ^= (1 << 9);  // pull scl low
    for(int i = 0; i < 10000;i++);
}

void half_T(void){
    for(int i = 0; i < 10000;i++);
    GPIOA->ODR |= (1 << 9);
    for(int i = 0; i < 10000;i++);
    GPIOA->ODR ^= (1 << 9);
}

void manual_i2c(void* args){
    (void) args;
    uint8_t address = 0x68;
    uint8_t WHO_AM_I = 0xFF;
    start_condition();



    // first frame
    for(int i = 6; i >= 0; --i){
        uint8_t temp = (address>>i) & 1; // 0b 0110 1000  this grabs 0b 0100 0000
        if(temp){
            GPIOA->ODR |= temp<<10;
        } else {
            GPIOA->ODR &= ~(1 << 10);
        }
        for(int i = 0; i < 10000;i++);
        half_T();
        for(int i = 0; i < 10000;i++);
    }


    // read/write bit
    // for(int i = 0; i < 10000;i++);
    GPIOA->ODR |= (1 << 10); 
    //
    for(int i = 0; i < 10000;i++);
    half_T();
    for(int i = 0; i < 10000;i++);
    half_T(); // ack/nack
    // end of firs frame

    for(int i = 7; i >= 0; --i){
        uint8_t temp = (WHO_AM_I>>i) & 1;
        if(temp){
            GPIOA->ODR |= temp<<10;
        } else {
            GPIOA->ODR &= ~(1 << 10);
        }
        for(int i = 0; i < 10000;i++);
        half_T();
        for(int i = 0; i < 10000;i++);
    }

    GPIOA->ODR |= (1 << 10); 
    // for(int i = 0; i < 10000;i++);
    // half_T();
    // for(int i = 0; i < 10000;i++);
    // for(int i = 0; i < 10000;i++);
    // GPIOA->ODR &= ~(1 << 10); 
    // half_T(); // should be 2nd ack
    //
    // // for(int i = 0; i < 8; i++){
    // //     half_T();
    // // }

}
