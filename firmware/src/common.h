#ifndef COMMON_H
#define COMMON_H

#define F_CPU 1000000UL
#define BAUD 2400UL

#include <util/delay.h>
#include <util/setbaud.h>
#include <stdint.h>
#include <avr/io.h>

/*definitions common to all modules
 */

//Bit operations
#define setbit(byte, bit) ((byte) |= ((1) << (bit)))
#define clearbit(byte, bit) ((byte) &= ~((1) << (bit)))
#define togglebit(byte, bit) ((byte) ^= ((1) << (bit)))
#define testbit(byte, bit) (((byte) >> (bit)) & (1))    //returns 1 or 0

//IC4.SCL:
#define PORT_IOSCL  PORTD
#define PIOSCL      PD3
#define DDR_IOSCL   DDRD
#define DDIOSCL     DDD3

#endif
