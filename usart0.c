/*
 * usart0.c
 *
 *  Created on: Oct 6, 2021
 *      Author: saranyan
 */

#include <avr/interrupt.h>
#include "common_preprocessors.h"
#include "usart0.h"

static char * data;

void USART0_Init(unsigned int ubrr)
{
	//---------Set baud rate---------//
	ubrr = ubrr + 1;					/*ubrr correction*/
	UBRR0H = (unsigned char) (ubrr >> 8);
	UBRR0L = (unsigned char) ubrr;

	//-----Control Register UCSRnA---//
	CLEAR_BIT(UCSR0A, MPCM0);			/*Disable Multi processor comm mode*/
	CLEAR_BIT(UCSR0A, U2X0); 			/*Clock Divider is 16*/
	CLEAR_BIT(UCSR0A, TXC0); 			/*No Transmission now*/

	//-----Control Register UCSRnB---//
	CLEAR_BIT(UCSR0B, UCSZ02);			/*Data length is 8 bit*/
	CLEAR_BIT(UCSR0B, RXEN0);			/*Disabled USART0 Receiver*/
	SET_BIT(UCSR0B, UDRIE0);			/*Enabled - Data Buffer Empty Interrupt*/
	SET_BIT(UCSR0B, TXCIE0);			/*Enabled - Data transmission completed Interrupt*/
	CLEAR_BIT(UCSR0B, RXCIE0);			/*Disabled - Data receive completed Interrupt*/

	//-----Control Register UCSRnC---//
	CLEAR_BIT(UCSR0C, UCPOL0);			/*Async Mode only*/
	/*Data Length*/
	SET_BIT(UCSR0B, UCSZ00);			/*Data length is 8 bit*/
	SET_BIT(UCSR0B, UCSZ01);			/*Data length is 8 bit*/
	CLEAR_BIT(UCSR0B, USBS0);			/*One Stop Bit*/
	/*Parity*/
	CLEAR_BIT(UCSR0B, UPM00);			/*Parity - Even*/
	SET_BIT(UCSR0B, UPM01);				/*Parity - Even*/
	/*Mode Selection*/
	CLEAR_BIT(UCSR0B, UMSEL00);			/*Mode - Async Mode*/
	CLEAR_BIT(UCSR0B, UMSEL01);			/*Mode - Async Mode*/

	//-------Turn On Transmitter-----//
	SET_BIT(UCSR0B, TXEN0);				/*Enabled USART0 Transmitter*/
}

void USART0_Send_Data(char * _data)
{
	data = _data;
	SET_BIT(UCSR0B, UDRIE0);			/*Enabled - Data Buffer Empty Interrupt*/
}

ISR(USART_UDRE_vect)
{
	if(*data != '\0')
	{
		UDR0 = *(data++);				/*Load Next Data Byte*/
	}
	else
	{
		UDR0 = '\n';
		CLEAR_BIT(UCSR0B, UDRIE0);		/*Disabled - Data Buffer Empty Interrupt*/
	}
}

ISR(USART_TX_vect)
{
	/* The TXCn Flag bit is automatically cleared,
	 * when a transmit complete interrupt is executed
	 */
}

