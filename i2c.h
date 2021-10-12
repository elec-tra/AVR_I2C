/*
 * i2c.h
 *
 *  Created on: Oct 5, 2021
 *      Author: saranyan
 */
#ifndef I2C_H_
#define I2C_H_

char data_buffer[19];

//-----Function Declarations------//
void I2C_Init();
void I2C_Start_Communication(uint8_t slave_addr);
void I2C_Stop_Communication();

#endif /* I2C_H_ */
