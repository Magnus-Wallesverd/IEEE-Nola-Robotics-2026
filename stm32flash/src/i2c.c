#include "i2c.h"
#include "rcc.h"
#include "gpio.h"
#include <stdint.h>

/*
// Initialize the I2C peripheral with basic settings
void I2C_Init(I2C_TypeDef *I2Cx, uint32_t timing) {
    RCC->APB1ENR |= (1 << 21);
    // Configure timing
    I2Cx->TIMINGR = timing;
    // Enable peripheral
    I2Cx->CR1 |= (1 << 0);  // PE = 1
}
*/



void master_receive(I2C_TypeDef* I2Cx, uint8_t nbytes, uint16_t slave_addr){
    I2Cx->CR2 |= (nbytes << 16)|(1 << 25)|(slave_addr << 1);
}

void I2C_Init(I2C_TypeDef* I2Cx, uint8_t mode, uint8_t nbytes, uint16_t slave_addr){
    switch(mode){
        case 0:
            I2Cx->CR1 &= ~(1<<0);
            I2Cx->CR1 |= (1<<1);
            // I2Cx->TIMINGR = (1 << 28)|(0x4 << 20)|(0x2 << 16)|(0xF << 8)|0x13;
            I2Cx->TIMINGR = 0x00303D5B;
            I2Cx->CR1 |= (1<<0);
            // master_receive(I2Cx, nbytes, slave_addr);
            I2Cx->CR2 = ~(1<<10)&((nbytes << 16)|(slave_addr << 1)|(1 << 13));
            I2C_Write(I2Cx, 0x75);
            I2C_Read(I2Cx);
        // case 1:
        // case 2:
    }
}

void I2C_Write(I2C_TypeDef* I2Cx, uint8_t reg){
    while (!(I2Cx->ISR & (1 << 1)));
    I2Cx->TXDR = reg;
    while (!(I2Cx->ISR & (1 << 6)));
}

void I2C_Read(I2C_TypeDef* I2Cx){
    I2Cx->CR2 |= (1<<10);   //read
    I2Cx->CR2 |= (1<<13);   //start
    while(I2Cx->ISR & (1<<2));
    uint8_t data = I2Cx->RXDR;
//     if(I2Cx->CR1 & 1){
//         I2Cx->CR2 |= (1<<13);
//         while(I2Cx->ISR & (1<<2));
//     }
}

// Send a start condition and address a device for writing
// void I2C_Write(I2C_TypeDef *I2Cx, uint8_t dev_addr, const uint8_t *data, uint16_t size) {
//
//     // 1. Set up transfer: address, size, write mode, start
//     // SADD[0] SADD[8] SADD[9] are dont care x.
//     I2Cx->CR2 = (dev_addr << 1) | (size << 16); // SADD[7:1], NBYTES[23:16]
//     I2Cx->CR2 &= ~(1 << 10); // RD_WRN = 0 (write)
//     I2Cx->CR2 |= (1 << 13);  // START
//
//     for (uint16_t i = 0; i < size; i++) {
//         // Wait until TXIS (Transmit interrupt status) is set
//         while (!(I2Cx->ISR & (1 << 1)));
//         // Write data
//         I2Cx->TXDR = data[i];
//     }
//     // Wait until TC (Transfer Complete) is set
//     while (!(I2Cx->ISR & (1 << 6)));
//     // Send STOP
//     I2Cx->CR2 |= (1 << 14);
//     // Wait until STOPF (Stop detection flag) is set
//     while (!(I2Cx->ISR & (1 << 5)));
//     // Clear STOPF by writing to ICR
//     I2Cx->ICR |= (1 << 5);
// }
/*
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
*/

// Write a single byte to a device register
// void I2C_WriteReg(I2C_TypeDef *I2Cx, uint8_t dev_addr, uint8_t reg_addr, uint8_t value) {
//
//     // Send the register address, then the data byte(s)
//     buf[0] = reg_addr;
//     buf[1] = value;
//     I2C_Write(I2Cx, dev_addr, buf, 2);
// }

// Read a single byte from a device register
// void I2C_ReadReg(I2C_TypeDef *I2Cx, uint8_t dev_addr, uint8_t reg_addr, uint8_t *value) {
//
//     // Write the register address (no STOP)
//     I2Cx->CR2 = (dev_addr << 1) | (1 << 16); // NBYTES = 1
//     I2Cx->CR2 &= ~(1 << 10); // Write mode
//     I2Cx->CR2 |= (1 << 13); // START
//
//     while(!(I2Cx->ISR & (1 << 1))); // Wait TXIS
//     I2Cx->TXDR = reg_addr;
//     while(!(I2Cx->ISR & (1 << 6))); // Wait TC (not STOP)
//
//     // Issue a repeated START to read data
//     I2Cx->CR2 = (dev_addr << 1) | (1 << 10) | (1 << 13) | (1 << 16); // Read mode, NBYTES=1, START
//
//     while(!(I2Cx->ISR & (1 << 2)));   // Wait RXNE
//     *value = (uint8_t)(I2Cx->RXDR & 0xFF);
//
//     while(!(I2Cx->ISR & (1 << 6)));
//     I2Cx->CR2 |= (1 << 14); // STOP
//     while(!(I2Cx->ISR & (1 << 5)));
//     I2Cx->ICR |= (1 << 5);
// }

void I2C_WriteReg_Wrapper(void* args){
    (void) args;
    // I2C_WriteReg(I2C1, BNO055, 0x10, 0xFF);
}
