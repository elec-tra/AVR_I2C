/*
 * general_utility_functions.c
 *
 *  Created on: Oct 7, 2021
 *      Author: saranyan
 */

#include <stdint.h>
#include "general_utility_functions.h"

char * Byte_to_HexChar(uint8_t _register, char * _p)
{
    char const digit[] = "0123456789";
    char * p = _p;
    uint8_t i = _register;
//    if(i<0)
//    {
//    	*p++ = '-';
//        i *= -1;
//    }
    uint8_t shifter = i;
    do{
    	++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{
    	*--p = digit[i%10];
        i = i/10;
    }while(i);

    return _p;
}

