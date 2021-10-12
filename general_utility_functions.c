/*
 * general_utility_functions.c
 *
 *  Created on: Oct 7, 2021
 *      Author: saranyan
 */

#include <stdint.h>
#include "general_utility_functions.h"

/*
 * Function: To print a byte in Hex, Unsigned Int & Char
 * -----------------------------------------------------
 * 1. Converting a byte to Hex characters.
 * 2. Converting the same byte to Unsigned Integer characters.
 * 3. Converting the same byte to ASCII.
 * Finally returns formatted string literal pointer
 *
 * _register: byte to print
 * _p: pointer to string literal
 *
 * returns: It returns formatted string literal pointer
 */
char * Debug_Byte(uint8_t _register, char * _p)
{
	_p = "Hex: 0x00     U_Int: 000     ASCII: 0\0";
	//----------Byte to Hex----------//
	const char hex_list[] = "0123456789ABCDEF";
	uint8_t temp = _register;
	_p[8] = hex_list[temp % 16];
	temp = temp / 16;
	_p[7] = hex_list[temp % 16];

	//----Byte to Unsigned Integer---//
	temp = _register;
	_p[23] = hex_list[temp % 10];
	temp = temp / 10;
	_p[22] = hex_list[temp % 10];
	temp = temp / 10;
	_p[21] = hex_list[temp % 10];

	//---------Byte to ASCII---------//
	_p[36] = (unsigned char) _register;

	return _p;
}

/*
 * Function: Convert a Integer to Characters
 * -----------------------------------------
 * In order to print integer via usart, the integer value
 * must be converted to characters(human readable)
 *
 * integer_value: value to be converted to characters
 * _p: pointer to string literal
 *
 * returns: A character pointer pointing to character array.
 */
char * Integer_to_Characters(int integer_value, char * _p)
{
    const char digit_list[] = "0123456789";
    register char * p = _p;
    int temp = integer_value;
    if(temp < 0)
    {
    	*p++ = '-';
        temp *= -1;
    }
    int _temp = temp;
    do{
    	++p;
        _temp = _temp / 10;
    }while(_temp);
    *p = '\0';
    do{
    	*--p = digit_list[temp % 10];
        temp = temp / 10;
    }while(temp);

    return _p;
}
