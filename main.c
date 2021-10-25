/*
 * main.c
 *
 *  Created on: Oct 5, 2021
 *      Author: saranyan
 */

#include <avr/io.h>
#include <util/delay.h>
#include "common_preprocessors.h"
#include "general_utility_functions.h"
#include "power_management.h"
#include "usart0.h"
#include "i2c.h"

/*I2C*/
#define SLAVE_ADDRESS 0b1101000

/*USART0*/
#define FOSC 16000000 					/*MCU Clock Speed*/
#define BAUD 115200						/*Required Baud Rate*/
#define MYUBRR ((FOSC/16/BAUD)-1)

/*I2C Example - DS3231*/
void Print_Seconds();

int main(void)
{
	//-------------Inits-------------//
	/*POWER MANAGEMENT*/
	DISABLE_ADC; ENABLE_I2C;			/*Disable ADC*/ /*ENABLE I2C*/
	DISABLE_SPI; DISABLE_TIMER0;		/*Disable SPI*/ /*DISABLE TIMER0*/
	DISABLE_TIMER1; DISABLE_TIMER2;		/*Disable TIMER1*/ /*DISABLE TIMER2*/
	ENABLE_USART0;						/*Enable USART0*/
	/*GPIO*/
	SET_BIT(DDRB, PB5);					/*Pin PB5 as Output*/
	CLEAR_BIT(PORTB, PB5);				/*Clear PB5 Bit*/
	/*UART0*/
	USART0_Init(MYUBRR);
	/*I2C*/
	I2C_Init();
	/*Interrupt*/
	SET_BIT(SREG, 7);					/*Enable Interrupt*/

	/*USART0 Transmit Example*/
	USART0_Send_Data("Serial Test Message...");
	_delay_ms(5);

	/*Start I2C for first time*/
	I2C_Start_Communication(SLAVE_ADDRESS);

	//-----------Event loop----------//
	while (1)
	{
		/*Blink LED Example*/
		TOGGLE_BIT(PORTB, PB5);

		_delay_ms(50);

		if(TWCR & (1 << TWINT))
		{
			if(I2C_Do_Communication() == I2C_COMPLETED)
			{
				I2C_Start_Communication(SLAVE_ADDRESS);

				/*Print Seconds Example - DS3231*/
				_delay_ms(1);			/*Just for USART0 complete its previous message*/
				Print_Seconds();
			}
			else
				;//Till not completed, so, wait
		}
	}
	return(0);
}

void Print_Seconds()
{
	char seconds[3];
	char string_pointer[3];
	//BCD to Integer conversion
	Integer_to_Characters((uint8_t) (i2c_data_buffer[0] >> 4), string_pointer);
	seconds[0] = string_pointer[0];
	//BCD to Integer conversion
	Integer_to_Characters((uint8_t) (i2c_data_buffer[0] & 0b00001111), string_pointer);
	seconds[1] = string_pointer[0];
	seconds[2] = '\0';

	USART0_Send_Data(seconds);
}
