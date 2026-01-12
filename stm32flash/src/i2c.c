/* TODO 
 * add a callback for device registry
 * proper I2C_Init
 * DMA
 */
#include "i2c.h"
#include "rcc.h"
#include "gpio.h"
#include <stdint.h>

int rx_i = 0;

void I2C1_EV_IRQHandler(void){

    if(I2C1->ISR & TXIS){
        I2C1->TXDR = 0;
        return;
    }

    if(I2C1->ISR & TC){
        I2C1->CR2 = (1 << 25)|(NBYTES << 16)|READ|(BNO055 << 1);
        I2C1->CR2 |= START;
        return;
    }
    
    if(I2C1->ISR & RXNE){
        buf[rx_i++] = I2C1->RXDR;
        return;
    }

    if(I2C1->ISR & STOPF){
        I2C1->ICR |=STOPCF;
        rx_i = 0;
        return;
    }
}

void I2C_Init(I2C_TypeDef* I2Cx, uint8_t mode){

    switch(mode){
        case 0:
            SetPinAlternate(GPIOB,0xC0);            // Set pins 6 & 7 to AF mode I2c
            AlternateFunctionSet(GPIOB,0xC0,4);      // set pins PB 6&7 to AF4
            SetOutputType(GPIOB, 0xC0, 1);
            SetOutputSpeed(GPIOB,0xC0, 1);

            I2Cx->CR1 &= ~(1<<0);
            I2Cx->CR1 |= TXIE|RXIE|TCIE|STOPIE;
            I2Cx->TIMINGR = 0x10420F13;
            I2Cx->ICR = 0x3F38;
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
    I2Cx->CR2 |= START;
    while (!(I2Cx->ISR & (1 << 1)));
    I2Cx->TXDR = reg;
    while (!(I2Cx->ISR & (1 << 6)));
}

void I2C_Read(I2C_TypeDef* I2Cx, uint8_t slave_addr, uint8_t nbytes){
    I2Cx->ICR = 0x3F38;
    I2Cx->CR2 = (1 << 25)|(nbytes << 16)|(1 << 10)|(slave_addr << 1);
    I2Cx->CR2 |= START;   //start
    while(!(I2Cx->ISR & (1<<2)));
    buf[0] = I2Cx->RXDR;
    while(!(I2Cx->ISR & (1<<5)));
}

void I2C_Write_Read(I2C_TypeDef* I2Cx, uint8_t slave_addr, uint8_t nbytes, uint8_t reg){
    I2C_Write(I2Cx, slave_addr, nbytes, reg);
    I2C_Read(I2Cx, slave_addr, nbytes);
}

void Sensor_Read(I2C_TypeDef* I2Cx){
    I2Cx->CR2 = (1 << 16)|(BNO055 << 1);
    I2Cx->CR2 &= ~(1<<25);
    I2Cx->CR2 &= ~(1 << 10);
    I2Cx->CR2 |= START;   //start
}

void Sensor_Read_Wrapper(void* args){
    (void) args;
    Sensor_Read(I2C1);
}
