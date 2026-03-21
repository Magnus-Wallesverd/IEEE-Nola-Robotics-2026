/**
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "vl53l1_platform.h"
#include "i2c.h"
#include "tcb.h"
#include "lock.h"
#include <string.h>
#include <time.h>
#include <math.h>

uint8_t ToF_tx_buffer[256];
uint8_t ToF_rx_buffer[256];

int8_t VL53L1_WriteMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count) {
	uint8_t status = 255;
	
    if(sizeof(index)+count > 255){
        return status;
    }

    ToF_tx_buffer[0] = index >> 8;
    ToF_tx_buffer[1] = index & 0xFF;
	
    for(uint32_t i = 0; i < count; i++){
        ToF_tx_buffer[i+sizeof(index)] = pdata[i];
    }

    Sensor_Write(I2C1, dev, ToF_tx_buffer, count);

    VL53L1_WaitMs(dev, 1);

	return status;
}

//need 
int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count){
	uint8_t status = 255;
	
    if(sizeof(index)+count > 255){
        return status;
    }

    ToF_tx_buffer[0] = index >> 8;
    ToF_tx_buffer[1] = index & 0xFF;
	
    for(uint32_t i = 0; i < count; i++){
        ToF_tx_buffer[i+sizeof(index)] = pdata[i];
    }

    Sensor_Write(I2C1, dev, ToF_tx_buffer, count);

    VL53L1_WaitMs(dev, 1);

	return status;
}

//need
int8_t VL53L1_WrByte(uint16_t dev, uint16_t index, uint8_t data) {

	uint8_t status = 255;
	
    ToF_tx_buffer[0] = index >> 8;
    ToF_tx_buffer[1] = index & 0xFF;
    ToF_tx_buffer[2] = data;

    Sensor_Write(I2C1, dev, ToF_tx_buffer, sizeof(index)+sizeof(data));   

    VL53L1_WaitMs(dev, 1);

	status = 0;

	return status;
}

//need
int8_t VL53L1_WrWord(uint16_t dev, uint16_t index, uint16_t data) {

	uint8_t status = 255;
	
    ToF_tx_buffer[0] = index >> 8;
    ToF_tx_buffer[1] = index & 0xFF;
    ToF_tx_buffer[2] = data >> 8;
    ToF_tx_buffer[3] = data & 0xFF;

    Sensor_Write(I2C1, dev, ToF_tx_buffer, sizeof(index)+sizeof(data));   

    VL53L1_WaitMs(dev, 1);

	status = 0;
	
	return status;
}

//need
int8_t VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data) {

	uint8_t status = 255;
	
    ToF_tx_buffer[0] = index >> 8;
    ToF_tx_buffer[1] = index & 0xFF;

    ToF_tx_buffer[2] = data >> 24;
    ToF_tx_buffer[3] = (data & 0xFF << 16) >> 16;
    ToF_tx_buffer[4] = (data & 0xFF << 8) >> 8;
    ToF_tx_buffer[5] = data & 0xFF;

    Sensor_Write(I2C1, dev, ToF_tx_buffer, sizeof(index)+sizeof(data));   

    VL53L1_WaitMs(dev, 1);

	status = 0;
	
	return status;
}

//need
int8_t VL53L1_RdByte(uint16_t dev, uint16_t index, uint8_t *data) {
	uint8_t status = 255;
	
    ToF_tx_buffer[0] = index >> 8;
    ToF_tx_buffer[1] = index & 0xFF;

    Sensor_Read(I2C1, dev, ToF_tx_buffer, sizeof(index), ToF_rx_buffer, 1);   
    VL53L1_WaitMs(dev, 1);

    *data = ToF_rx_buffer[0];
	
    status = 0;
	
	return status;
}

//Need
int8_t VL53L1_RdWord(uint16_t dev, uint16_t index, uint16_t *data) {
	uint8_t status = 255;
	
    ToF_tx_buffer[0] = index >> 8;
    ToF_tx_buffer[1] = index & 0xFF;

    Sensor_Read(I2C1, dev, ToF_tx_buffer, sizeof(index), ToF_rx_buffer, sizeof(uint16_t));   

    VL53L1_WaitMs(dev, 1);

    *data = ToF_rx_buffer[0]<<8 | ToF_rx_buffer[1];
	status = 0;
	
	return status;
}

//need
int8_t VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *data) {
	uint8_t status = 255;
	
    ToF_tx_buffer[0] = index >> 8;
    ToF_tx_buffer[1] = index & 0xFF;

    Sensor_Read(I2C1, dev, ToF_tx_buffer, sizeof(index), ToF_rx_buffer, sizeof(uint32_t));   

    VL53L1_WaitMs(dev, 1);

    *data = ToF_rx_buffer[0]<<24 | ToF_rx_buffer[1]<<16 | ToF_rx_buffer[2] << 8 | ToF_rx_buffer[3];

	status = 0;

	return status;
}

int8_t VL53L1_WaitMs(uint16_t dev, int32_t wait_ms){
	uint8_t status = 255;
	
    I2C_Wait(I2C1);

    status = 0;
	
	return status;
}
