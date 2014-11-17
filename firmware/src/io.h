#ifndef IO_H
#define IO_H

#include <avr/io.h>

/*input/output panel connections
 */
//CLK: 
#define PORT_IOCLK  PORTD
#define PIOCLK      PD2
#define DDR_IOCLK   DDRD
#define DDIOCLK     DDD2
//DAT:
#define PORT_IODAT  PORTC
#define PIODAT      PC5
#define DDR_IODAT   DDRC
#define DDIODAT     DDC5
//KEY:
#define PORT_IOKEY  PORTC
#define PIOKEY      PC4
#define DDR_IOKEY   DDRC
#define DDIOKEY     DDC4
//LEDC:
#define PORT_IOLED  PORTC
#define PIOLED      PC3
#define DDR_IOLED   DDRC
#define DDIOLED     DDC3
//DIS0:
#define PORT_IODIS0 PORTB
#define PIODIS0     PB6
#define DDR_IODIS0  DDRB
#define DDIODIS0    DDB6
//DIS1:
#define PORT_IODIS1 PORTB
#define PIODIS1     PB7
#define DDR_IODIS1  DDRB
#define DDIODIS1    DDB7
//SCL:
#define PORT_IOSCL  PORTD
#define PIOSCL      PD3
#define DDR_IOSCL   DDRD
#define DDIOSCL     DDD3

void io_init(void);

#endif
