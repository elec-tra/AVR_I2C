/*
 * i2c.c
 *
 *  Created on: Oct 5, 2021
 *      Author: saranyan
 */

#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include "common_preprocessors.h"
#include "usart0.h"
#include "i2c.h"
#include "general_utility_functions.h"

#define FOSC 16000000 					/*MCU Clock Speed*/
#define SCL_FREQUENCY 100				/*in Khz*/
#define PRE_SCALER 1					/*Refer Datasheet*/
#define TWBR_value (((FOSC/SCL_FREQUENCY)-16)/(2*PRE_SCALER))

#define WRITE_OPERATION(x) (x << 1) & ~(1 << 0)
#define READ_OPERATION(x) (x << 1) | (1 << 0)

static volatile uint8_t slave_address = 0xFF;
volatile char * p;

void I2C_Init()
{
	//-------Set SCl Frequency-------//
	TWBR = (unsigned char) TWBR_value;	/*Pre Scaler*/

	//-----Control Register TWCR-----//
	CLEAR_BIT(TWCR, TWIE);				/*Disable Interrupt*/
	CLEAR_BIT(TWCR, TWEN);				/*Disable TWI*/
	CLEAR_BIT(TWCR, TWSTO);				/*No Stop Signal*/
	CLEAR_BIT(TWCR, TWSTA);				/*No Start Signal*/
	CLEAR_BIT(TWCR, TWEA);				/*Disable Automatic Acknowledge*/
	CLEAR_BIT(TWCR, TWINT);				/*Ready for New Job*/

	//--------Set Pre-Scaler---------//
	CLEAR_BIT(TWSR, TWPS0);				/*Pre-Scaler is 1*/
	CLEAR_BIT(TWSR, TWPS1);				/*Pre-Scaler is 1*/

	//Set I2C Address for Master(MCU)//
	TWAR = 0b11111110;					/*MCU I2C Address*/
	CLEAR_BIT(TWAR, TWGCE);				/*Disable I2C General Call*/

	//---Set Address Mask Register--//
	TWAMR = 0b00000000;					/*Enable Address comparision for all Bits*/

	//----------Enable TWI----------//
	SET_BIT(TWCR, TWEN);				/*Enable TWI*/
}

void I2C_Start_Communication(uint8_t slave_addr)
{
	p = data_buffer;
	SET_BIT(TWCR, TWIE);				/*Enable Interrupt*/
	SET_BIT(TWCR, TWSTA);				/*Give Start Signal*/
	SET_BIT(TWCR, TWINT);				/*Ready for New Job*/
	slave_address = slave_addr;
}

ISR(TWI_vect)
{
	//-------Mater Transmitter------//
	if(TWSR == 0x08)					/*Start Signal Sent Succesfully*/
	{
		//USART0_Send_Data("1");

		//TWDR = WRITE_OPERATION(slave_address);
		TWDR = 0b11010000;
		CLEAR_BIT(TWCR, TWSTA);			/*No more Start Signal needed*/
		SET_BIT(TWCR, TWINT);			/*Continue the Job*/
	}
	if(TWSR == 0x18)					/*SLA+W has been transmitted Succesfully*/
	{
		//USART0_Send_Data("2");

		TWDR = 0x00;					/*Set Slave Address pointer to address 0x00*/
		SET_BIT(TWCR, TWINT);			/*Continue the Job*/
	}
	if(TWSR == 0x28)					/*Data  byte  has  been  transmitted Succesfully*/
	{
		//USART0_Send_Data("3");

		SET_BIT(TWCR, TWSTA);			/*Give Repeated Start Signal*/
		SET_BIT(TWCR, TWINT);			/*Continue the Job*/
	}
	if(TWSR == 0x10)					/*Repeated Start Signal sent Succesfully*/
	{
		//USART0_Send_Data("4");

		//CLEAR_BIT(TWCR, TWSTA);		/*No more Start Signal needed*/
		//TWDR = READ_OPERATION(slave_address);
		TWDR = 0b11010001;
		CLEAR_BIT(TWCR, TWSTA);			/*No more Start Signal needed*/
		SET_BIT(TWCR, TWINT);			/*Continue the Job*/
	}

	//--------Mater Receiver--------//
	if(TWSR == 0x40)
	{
		//USART0_Send_Data("5");

		SET_BIT(TWCR, TWEA);			/*Send Acknowledge*/
		SET_BIT(TWCR, TWINT);			/*Continue the Job*/
	}
	if(TWSR == 0x50)
	{
		//Received a byte from slave
		if((p - data_buffer) <= 16)
		{
			//USART0_Send_Data("6");

			*p++ = TWDR;
			SET_BIT(TWCR, TWINT);		/*Continue the Job*/
		}
		else if((p - data_buffer) == 17)
		{
			//USART0_Send_Data("7");

			CLEAR_BIT(TWCR, TWEA);		/*Send Not Acknowledge*/
			*p++ = TWDR;
			SET_BIT(TWCR, TWINT);		/*Continue the Job*/
		}
	}
	if(TWSR == 0x58)
	{
		*p = '\0';
		CLEAR_BIT(TWCR, TWSTO);			/*Send Stop Signal*/
		CLEAR_BIT(TWCR, TWIE);			/*Disable Interrupt*/
		SET_BIT(TWCR, TWINT);			/*Continue the Job*/
	}
}
void I2C_Stop_Communication()
{
	//----------Enable TWI----------//
	CLEAR_BIT(TWCR, TWEN);				/*Disable TWI*/
}

//USART0_Send_Data(Byte_to_HexChar((uint8_t) TWSR, debug_message));
//CLEAR_BIT(TWCR, TWIE);
//_delay_ms(2000);
//SET_BIT(TWCR, TWIE);

