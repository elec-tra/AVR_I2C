/*
 * main.c
 *
 *  Created on: Oct 5, 2021
 *      Author: saranyan
 */

#include <avr/io.h>
#include <util/delay.h>
#include "common_preprocessors.h"
#include "power_management.h"

int main(void)
{
	//-------------Inits-------------//
	/*POWER MANAGEMENT*/
	DISABLE_ADC; ENABLE_I2C;			/*Disable ADC*/ /*ENABLE I2C*/
	DISABLE_SPI; DISABLE_TIMER0;		/*Disable SPI*/ /*DISABLE TIMER0*/
	DISABLE_TIMER1; DISABLE_TIMER2;		/*Disable TIMER1*/ /*DISABLE TIMER2*/
	DISABLE_USART0;						/*Disable USART0*/
	/*GPIO*/
	SET_BIT(DDRB, PB5);					/*Pin PB5 as Output*/
	CLEAR_BIT(PORTB, PB5);				/*Clear PB5 Bit*/

	//-----------Event loop----------//
	while (1)
	{
		/*Blink LED Example*/
		TOGGLE_BIT(PORTB, PB5);
		_delay_ms(1000);
	}
	return(0);
}
