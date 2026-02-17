#include <stdint.h>
#include "i2c.h"

int rx_i = 0;
int nack_counter = 0;
int bno[] = {0x3d, 0xC, 0x1A};
volatile int bno_flag = 0;
int tx_i = 0;
uint8_t rx_buffer[I2C_BUFFER_SIZE];

uint8_t* get_i2c_buffer(void) {
    return rx_buffer;
}

void I2C1_EV_IRQHandler(void){

    if(I2C1->ISR & STOPF){
        I2C1->ICR |=STOPCF;
        rx_i = 0;
        tx_i = 0;
        return;
    } else if(I2C1->ISR & RXNE){
        rx_buffer[rx_i++] = I2C1->RXDR;
        return;
    } else if(I2C1->ISR & TXIS){
        if(bno_flag){
            I2C1->TXDR = bno[2];
        } else {
            I2C1->TXDR = bno[tx_i++];
        }
        return;
    } else if(I2C1->ISR & TC){
        I2C1->CR2 = 0;
        I2C1->CR2 |= (1 << 25)|(NBYTES << 16)|READ|(BNO055 << 1)|START;
        return;
    } else if(I2C1->ISR & NACKF){
        nack_counter++;
        return;
    } else {
        uint32_t i2c_isr = I2C1->ISR;
        __asm volatile("BKPT #6"); // print the i2c interrupt flags
    }
    
}

void I2C1_ER_IRQHandler(void){
    if(I2C1->ISR & NACKF){
        __asm volatile("BKPT #6");
    }
}
