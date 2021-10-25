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
#include "general_utility_functions.h"
#include "usart0.h"
#include "i2c.h"

#define FOSC 16000000 					/*MCU Clock Speed*/
#define SCL_FREQUENCY 100				/*in Khz*/
#define PRE_SCALER 1					/*Refer Datasheet*/
#define TWBR_value (((FOSC/SCL_FREQUENCY)-16)/(2*PRE_SCALER))

#define WRITE_OPERATION(x) (x << 1) & ~(1 << 0)
#define READ_OPERATION(x) (x << 1) | (1 << 0)

static volatile uint8_t slave_address = 0xFF;
volatile char * p;

/*
 * Function: Initialize I2C(i.e TWI) Peripheral
 * --------------------------------------------
 * Set and Configure registers of I2C
 * peripheral for Master transmit and receive.
 *
 * returns: It returns nothing.
 */
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
	TWAMR = 0b00000000;					/*Enable Address comparison for all Bits*/

	//----------Enable TWI----------//
	SET_BIT(TWCR, TWEN);				/*Enable TWI*/
}

/*
 * Function: Starts I2C(i.e TWI) Peripheral
 * ----------------------------------------
 * Assigns a character pointer to I2C Data
 * buffer variable(i.e i2c_data_buffer)
 *
 * slave_addr: 7-bit address of target slave
 *
 * returns: It returns nothing.
 */
void I2C_Start_Communication(uint8_t slave_addr)
{
	p = i2c_data_buffer;

	SET_BIT(TWCR, TWSTA);				/*Give Start Signal*/
	SET_BIT(TWCR, TWINT);				/*Ready for New Job*/
	slave_address = slave_addr;
}

/*
 * Function: Executes communication between master and slave
 * ---------------------------------------------------------
 * This makes decision based on I2C status. I2C status can be
 * determined with TWSR register.
 *
 * returns: I2C Communication status. 1 or 0
 * 			Note: "1" means I2C Communication was successful
 * 				  "0" means I2C Communication is not completed
 * 				   yet
 */
uint8_t I2C_Do_Communication()
{
	//-------Debug TWI Status-------//
	/*
	 * This is just for debug I2C status.
	 * So, The following three lines can
	 * be commented if debug is not
	 * needed
	 */
	char * debug_message;
	USART0_Send_Data(Debug_Byte((uint8_t) TWSR, debug_message));
	_delay_ms(4);

	//-------Mater Transmitter------//
	if(TWSR == 0x08)					/*Start Signal Sent Successfully*/
	{
		//-------Check TWI Status-------//
		/*
		 * The following line(USART0_Send_
		 * Data("Step 1");) is just for
		 * checking status of I2C.It can
		 * be if debug is not needed
		 */
		USART0_Send_Data("Step 1");

		TWDR = WRITE_OPERATION(slave_address);
										/*Configure the slave for writing into slave*/
		CLEAR_BIT(TWCR, TWSTA);			/*No more Start Signal needed*/
	}
	if(TWSR == 0x18)					/*SLA+W has been transmitted Successfully*/
	{
		USART0_Send_Data("Step 2");

		TWDR = 0x00;					/*Set Slave Address pointer to address 0x00*/
	}
	if(TWSR == 0x28)					/*Data  byte  has  been  transmitted Successfully*/
	{
		USART0_Send_Data("Step 3");

		SET_BIT(TWCR, TWSTA);			/*Give Repeated Start Signal*/
	}
	if(TWSR == 0x10)					/*Repeated Start Signal sent Successfully*/
	{
		USART0_Send_Data("Step 4");

		TWDR = READ_OPERATION(slave_address);
										/*Configure the slave for reading from slave*/
		CLEAR_BIT(TWCR, TWSTA);			/*No more Start Signal needed*/
	}

	//--------Mater Receiver--------//
	if(TWSR == 0x40)					/*Slave configure as transmitter successfully*/
	{
		USART0_Send_Data("Step 5");

		SET_BIT(TWCR, TWEA);			/*Enable Acknowledge for upcomming data*/
	}
	if(TWSR == 0x50)					/*Slave started transmitting data bytes*/
	{
		if((p - i2c_data_buffer) <= 16)	/*Receive all bytes except last byte - with acknowledgement*/
		{
			USART0_Send_Data("Step 6");

			*p++ = TWDR;				/*Store received bytes to buffer*/
		}
		else if((p - i2c_data_buffer) == 17)
										/*Receive last byte - without acknowledgement*/
		{
			USART0_Send_Data("Step 7");

			CLEAR_BIT(TWCR, TWEA);		/*Send Not Acknowledge - To stop slave Data transmit*/
			*p++ = TWDR;				/*Store last byte*/
		}
	}
	if(TWSR == 0x58)
	{
		USART0_Send_Data("Step 8");

		*p = '\0';						/*Terminate buffer with null character*/
		SET_BIT(TWCR, TWSTO);			/*Send Stop Signal*/
		SET_BIT(TWCR, TWINT);			/*Continue the Job*/
		return 1;						/*"1" means I2C Communication was successful*/
	}

	SET_BIT(TWCR, TWINT);				/*Continue the Job*/
	return 0;							/*"0" means I2C Communication is not completed yet*/
}

/*
 * Function: Disables I2C(i.e TWI) Peripheral
 * ------------------------------------------
 * It disable TWI peripheral
 *
 * returns: It returns nothing.
 */
void I2C_Stop_Communication()
{
	//----------Disable TWI----------//
	CLEAR_BIT(TWCR, TWEN);				/*Disable TWI*/
}
