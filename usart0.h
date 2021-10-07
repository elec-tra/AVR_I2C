/*
 * usart0.h
 *
 *  Created on: Oct 6, 2021
 *      Author: saranyan
 */

#ifndef USART0_H_
#define USART0_H_

//-----Function Declarations------//
void USART0_Init(unsigned int ubrr);
void USART0_Send_Data(char * _data);

#endif /* USART0_H_ */
