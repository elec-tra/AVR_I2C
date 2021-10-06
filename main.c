/*
 * main.c
 *
 *  Created on: Oct 5, 2021
 *      Author: saranyan
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "common_preprocessors.h"
#include "power_management.h"
#include "usart0.h"
#include "i2c.h"

/*I2C*/
#define Slave_Address 0b1101000

/*USART0*/
#define FOSC 16000000 					/*MCU Clock Speed*/
#define BAUD 115200						/*Required Baud Rate*/
#define MYUBRR ((FOSC/16/BAUD)-1)

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
	USART0_Send_Data("Serial Test Message...\n");
	_delay_ms(10);

	/*I2C Example*/
	I2C_Start_Communication(Slave_Address);

	//-----------Event loop----------//
	while (1)
	{
		/*Blink LED Example*/
		TOGGLE_BIT(PORTB, PB5);
		_delay_ms(1000);
	}
	return(0);
}
