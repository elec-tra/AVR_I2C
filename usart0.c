/*
 * usart0.c
 *
 *  Created on: Oct 6, 2021
 *      Author: saranyan
 */

#include <avr/interrupt.h>
#include "common_preprocessors.h"
#include "usart0.h"

static char serial_buffer[100];
static char * buffer_pointer;

/*
 * Function: Initialize USART0 Peripheral
 * --------------------------------------
 * Set and Configure registers of USART0
 * peripheral for serial transmission only.
 *
 * ubrr: Contains desired baud rate for user.
 *
 * returns: It returns nothing.
 */
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
	CLEAR_BIT(UCSR0B, UDRIE0);			/*Disabled - Data Buffer Empty Interrupt*/
	CLEAR_BIT(UCSR0B, TXCIE0);			/*Disabled - Data transmission completed Interrupt*/
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

	//-------Make Register Ready-----//
	UDR0 = '\0';						/*Clear Transmit Register*/

	//-------Turn On Transmitter-----//
	SET_BIT(UCSR0B, TXEN0);				/*Enabled USART0 Transmitter*/
}

/*
 * Function: Setup USART0 Peripheral for Transmission
 * --------------------------------------------------
 * Copies incoming character array to a dedicated
 * serial buffer.
 * Data Buffer Empty Interrupt is Enabled.
 * Data transmission completed Interrupt is Enabled.
 *
 * * _data: Pointer to incoming character array.
 *
 * returns: It returns nothing.
 */
void USART0_Send_Data( register char * _data)
{
	register uint8_t i = 0;
	while(*_data != '\0')				/*Load data to serial_buffer*/
	{
		serial_buffer[i++] = *(_data++);
	}
	serial_buffer[i] = '\n';
	buffer_pointer = serial_buffer;		/*Set serial buffer pointer*/

	SET_BIT(UCSR0B, UDRIE0);			/*Enabled - Data Buffer Empty Interrupt*/
	SET_BIT(UCSR0B, TXCIE0);			/*Enabled - Data transmission completed Interrupt*/
}

/*
 * Function: ISR for USART0 Transmit data register is empty
 * --------------------------------------------------------
 * This Interrupt Sub-Routine executed when USART0 transmit
 * data register is empty.
 * Disables data buffer empty interrupt after all bytes are
 * transmitted.
 *
 * retuns: It returns nothing.
 */
ISR(USART_UDRE_vect)
{
	if(*buffer_pointer != '\n')
	{
		UDR0 = *(buffer_pointer++);		/*Load Next Data Byte*/
	}
	else
	{
		UDR0 = '\n';
		CLEAR_BIT(UCSR0B, UDRIE0);		/*Disabled - Data Buffer Empty Interrupt*/
	}
}

/*
 * Function: ISR for USART0 Transmit shift register is empty
 * ---------------------------------------------------------
 * This Interrupt Sub-Routine executed when USART0 transmit
 * shift register is empty.
 * Disables data transmission completed interrupt after all
 * bytes are transmitted.
 *
 * retuns: It returns nothing.
 */
ISR(USART_TX_vect)
{
	/* The TXCn Flag bit is automatically cleared,
	 * when a transmit complete interrupt is executed
	 */

	CLEAR_BIT(UCSR0B, TXCIE0);			/*Disabled - Data transmission completed Interrupt*/
}
