/*
 * i2c.c
 *
 *  Created on: Oct 5, 2021
 *      Author: saranyan
 */

#include <avr/interrupt.h>
#include <util/delay.h>
#include "common_preprocessors.h"
#include "i2c.h"

#define FOSC 16000000 					/*MCU Clock Speed*/
#define SCL_FREQUENCY 100				/*in Khz*/
#define PRE_SCALER 1					/*Refer Datasheet*/
#define TWBR_value (((FOSC/SCL_FREQUENCY)-16)/(2*PRE_SCALER))

#define WRITE_OPERATION(x) (x << 1) & ~(1 << 0)
#define READ_OPERATION(x) (x << 1) | (1 << 0)

static volatile uint8_t slave_address = 0xFF;

void I2C_Init()
{
	//-------Set SCl Frequency-------//
	TWBR = (unsigned char) TWBR_value;		/*Pre Scaler*/

	//-----Control Register TWCR-----//
	CLEAR_BIT(TWCR, TWIE);				/*Disable Interrupt*/
	CLEAR_BIT(TWCR, TWEN);				/*Disable TWI*/
	CLEAR_BIT(TWCR, TWSTO);				/*No Stop Signal*/
	CLEAR_BIT(TWCR, TWSTA);				/*No Start Signal*/
	SET_BIT(TWCR, TWEA);				/*Enable Automatic Acknowledge*/
	CLEAR_BIT(TWCR, TWINT);				/*Ready for New Job*/

	//--------Set Pre-Scaler---------//
	CLEAR_BIT(TWSR, TWPS0);				/*Pre-Scaler is 1*/
	CLEAR_BIT(TWSR, TWPS1);				/*Pre-Scaler is 1*/

	//Set I2C Address for Master(MCU)//
	TWAR = 0b11111110;					/*MCU I2C Address*/
	CLEAR_BIT(TWAR, TWGCE);				/*Disable I2C General Call*/

	//---Set Address Mask Register--//
	TWAMR = 0b00000000;					/*Enable Address comparision for all Bits*/

	//-Enable TWI and Its Interrupt-//
	SET_BIT(TWCR, TWIE);				/*Enable Interrupt*/
	SET_BIT(TWCR, TWEN);				/*Enable TWI*/
}

void I2C_Start_Communication(uint8_t slave_addr)
{
	SET_BIT(TWCR, TWSTA);				/*Give Start Signal*/
	slave_address = slave_addr;
}

ISR(TWI_vect)
{
	static uint8_t temp = 0;

	char temp1[] = { (unsigned char) (TWDR-200), '\0' };
	USART0_Send_Data(temp1);
	_delay_ms(2000);

	//-------Mater Transmitter------//
	if(TWSR == 0x08)					/*Start Signal Sent Succesfully*/
	{
		TWDR = WRITE_OPERATION(slave_address);
		SET_BIT(TWCR, TWINT);			/*Ready for New Job*/
	}
	if(TWSR == 0x18)
	{
		if(temp == 0)
		{
			TWDR = 0x00;
			SET_BIT(TWCR, TWINT);			/*Continue the Job*/
			++temp;
		}
		if(temp == 1)
		{
			TWDR = READ_OPERATION(slave_address);
			SET_BIT(TWCR, TWINT);			/*Continue the Job*/
			++temp;
		}
	}

	//--------Mater Receiver--------//
	if(TWSR == 0x18)
	{
		//Received a byte from slave
		char tempa = (unsigned char) TWDR;
		USART0_Send_Data("Byte Received\n");
	}
}

//char temp1[] = { (unsigned char) (TWDR-100), '\0' };
//USART0_Send_Data(temp1);
//_delay_ms(2000);

