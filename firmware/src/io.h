#ifndef IO_H
#define IO_H

/*input/output panel connections (not soldered yet)
 */
//CLK: 
#define PORT_IOCLK
#define PIOCLK
#define DDR_IOCLK
#define DDIOCLK
//DAT:
#define PORT_IODAT
#define PIODAT
#define DDR_IODAT
#define DDIODAT
//KEY:
#define PORT_IOKEY
#define PIOKEY
#define DDR_IOKEY
#define DDIOKEY
//LEDC:
#define PORT_IOLED
#define PIOLED
#define DDR_IOLED
#define DDIOLED
//DIS0:
#define PORT_IODIS0
#define PIODIS0
#define DDR_IODIS0
#define DDIODIS0
//DIS1:
#define PORT_IODIS1
#define PIODIS1
#define DDR_IODIS1
#define DDIODIS1

void io_init(void);

#endif
