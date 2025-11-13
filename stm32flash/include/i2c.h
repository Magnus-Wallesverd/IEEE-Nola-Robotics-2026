//this is the i2c.h header file 
//


#ifndef I2C_H
#define I2C_H

#include <stdint.h>
// #include 

#define I2C_BUFFER_SIZE 2
#define BNO055 0x29

typedef struct {
    volatile uint32_t CR1;       // 0x00
    volatile uint32_t CR2;       // 0x04
    volatile uint32_t OAR1;      // 0x08
    volatile uint32_t OAR2;      // 0x0C
    volatile uint32_t TIMINGR;   // 0x10
    volatile uint32_t TIMEOUTR;  // 0x14
    volatile uint32_t ISR;       // 0x18
    volatile uint32_t ICR;       // 0x1C
    volatile uint32_t PECR;      // 0x20
    volatile uint32_t RXDR;      // 0x24
    volatile uint32_t TXDR;      // 0x28
} I2C_TypeDef;

// Peripheral base addresses (APB1)
#define I2C1 ((I2C_TypeDef *)0x40005400)
#define I2C2 ((I2C_TypeDef *)0x40005800)

extern uint8_t buf[I2C_BUFFER_SIZE];

// I2C HAL function prototypes

void I2C_Init(I2C_TypeDef *I2Cx, uint32_t timing);
void I2C_Write(I2C_TypeDef *I2Cx, uint8_t dev_addr, const uint8_t *data, uint16_t size);
void I2C_Read(I2C_TypeDef *I2Cx, uint8_t dev_addr, uint8_t *data, uint16_t size);
void I2C_WriteReg(I2C_TypeDef *I2Cx, uint8_t dev_addr, uint8_t reg_addr, uint8_t value);
void I2C_ReadReg(I2C_TypeDef *I2Cx, uint8_t dev_addr, uint8_t reg_addr, uint8_t *value);
void I2C_WriteReg_Wrapper(void* args);
void manual_i2c(void* args);

#endif // I2C_H

