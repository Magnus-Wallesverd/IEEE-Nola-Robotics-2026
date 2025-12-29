/* TODO 
 * proper I2C_Init
 * DMA
 */
#include "i2c.h"
#include "rcc.h"
#include "gpio.h"
#include <stdint.h>


void master_receive(I2C_TypeDef* I2Cx, uint8_t nbytes, uint16_t slave_addr){
    I2Cx->CR2 |= (nbytes << 16)|(1 << 25)|(slave_addr << 1);
}

void I2C_Init(I2C_TypeDef* I2Cx, uint8_t mode){
    switch(mode){
        case 0:
            I2Cx->CR1 &= ~(1<<0);
            I2Cx->CR1 |= (1<<1);
            I2Cx->TIMINGR = 0x10420F13;
            I2Cx->CR1 |= (1<<0);
        // case 1:
        // case 2:
    }
}

void I2C_Write(I2C_TypeDef* I2Cx, uint8_t slave_addr, uint8_t nbytes, uint8_t reg){
    I2Cx->ICR = 0x3F38;
    I2Cx->CR2 = (nbytes << 16)|(slave_addr << 1);
    I2Cx->CR2 &= ~(1<<25);
    I2Cx->CR2 &= ~(1 << 10);
    I2Cx->CR2 |= (1<<13);   //start
    while (!(I2Cx->ISR & (1 << 1)));
    I2Cx->TXDR = reg;
    while (!(I2Cx->ISR & (1 << 6)));
}

void I2C_Read(I2C_TypeDef* I2Cx, uint8_t slave_addr, uint8_t nbytes){
    I2Cx->ICR = 0x3F38;
    I2Cx->CR2 = (1 << 25)|(nbytes << 16)|(1 << 10)|(slave_addr << 1);
    I2Cx->CR2 |= (1<<13);   //start
    while(!(I2Cx->ISR & (1<<2)));
    buf[0] = I2Cx->RXDR;
    while(!(I2Cx->ISR & (1<<5)));
}

void I2C_Write_Read(I2C_TypeDef* I2Cx, uint8_t slave_addr, uint8_t nbytes, uint8_t reg){
    I2C_Write(I2Cx, slave_addr, nbytes, reg);
    I2C_Read(I2Cx, slave_addr, nbytes);
}

void I2C_WriteReg_Wrapper(void* args){
    (void) args;
    // I2C_WriteReg(I2C1, BNO055, 0x10, 0xFF);
}
