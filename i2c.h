/*
 * i2c.h
 *
 *  Created on: Oct 5, 2021
 *      Author: saranyan
 */
#ifndef I2C_H_
#define I2C_H_

#define I2C_COMPLETED 1
#define I2C_NOT_COMPLETED 0

//-----Variable Declarations------//
char i2c_data_buffer[19];

//-----Function Declarations------//
void I2C_Init();
void I2C_Start_Communication(uint8_t slave_addr);
uint8_t I2C_Do_Communication();
void I2C_Stop_Communication();

#endif /* I2C_H_ */
