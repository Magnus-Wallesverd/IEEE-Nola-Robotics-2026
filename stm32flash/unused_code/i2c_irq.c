#include <stdint.h>
#include "i2c.h"

int i2c_rx_i = 0;
int i2c_tx_i = 0;
int nack_counter = 0;
int bno[] = {OPR_REG, NDOF_MODE, HEADING_REG};
int bno_flag = 0;

uint8_t i2c_rx_buffer[I2C_BUFFER_SIZE];

uint8_t* get_i2c_buffer(void) {
    return i2c_rx_buffer;
}

void I2C1_EV_IRQHandler(void){

    if(I2C1->ISR & STOPF){
        I2C1->ICR |=STOPCF;
        i2c_rx_i = 0;
        i2c_tx_i = 0;
        return;
    } else if(I2C1->ISR & RXNE){
        i2c_rx_buffer[i2c_rx_i++] = I2C1->RXDR;
        return;
    } else if(I2C1->ISR & TXIS){
        bno_ready = 1;
        if(bno_flag){
            I2C1->TXDR = bno[2];
        } else {
            I2C1->TXDR = bno[i2c_tx_i++];
        }
        return;
    } else if(I2C1->ISR & TC){
        I2C1->CR2 = 0;
        I2C1->CR2 |= (1 << 25)|(NBYTES << 16)|READ|(BNO055 << 1)|START;
        return;
    } else if(I2C1->ISR & NACKF){
        nack_counter++;
        bno_ready = 0;
        I2C1->ICR |= NACKCF;
        return;
    } else {
        uint32_t i2c_isr = I2C1->ISR;
    }
    
}

void I2C1_ER_IRQHandler(void){
    // if(I2C1->ISR & NACKF){
    //     __asm volatile("BKPT #6");
    // }
}
