#include <avr/io.h>
#include "io.h"
#include "common.h"
#include "timer.h"
#include "uart.h"

/*fan connection
 */
//connected to PC0
#define PORT_FAN    PORTC
//pin number of fan in PORT_FAN
#define PFAN        PC0
#define DDR_FAN     DDRC
#define DDFAN       DDC0

/*compressor connection
 */
//connected to PD4
#define PORT_COMP   PORTD
//pin number of compressor in PORT_COMP
#define PCOMP       PD4
#define DDR_COMP    DDRD
#define DDCOMP      DDD4

/*ambient temperature sensor
 */
#define PORT_ATS    PORTC
#define PATS        PC3
#define DDR_ATS     DDRC
#define DDATS       DDC3

/*humidity sensor
 */
#define PORT_HUM    PORTC
#define PHUM        PC2
#define DDR_HUM     DDRC
#define DDHUM       DDC2

/*cooling unit temperature sensor
 */
#define PORT_CTS    PORTC
#define PCTS        PC1
#define DDR_CTS     DDRC
#define DDCTS       DDC1

/*water full sensor
 */
#define PORT_FULL   PORTB
#define PFULL       PB0
#define DDR_FULL    DDRB
#define DDFULL      DDB0


//Fan control routines
void start_fan(void)
{
    setbit(PORT_FAN, PFAN);
}

void stop_fan(void)
{
    clearbit(PORT_FAN, PFAN);
}

void toggle_fan(void)
{
    togglebit(PORT_FAN, PFAN);
}

//Compressor control routines
void start_comp(void)
{
    setbit(PORT_COMP, PCOMP);
}

void stop_comp(void)
{
    clearbit(PORT_COMP, PCOMP);
}

void toggle_comp(void)
{
    togglebit(PORT_COMP, PCOMP);
}

void init(void) {
    uart_init();
    //make compressor and fan outputs
    setbit(DDR_FAN, DDFAN);
    setbit(DDR_COMP, DDCOMP);
    //initialize timer (needed by io_init())
    timer_init();
    //initialize input/output panel
    io_init();

    //everything is set up, globally enable interrupts
    sei();
}

int main(void)
{
    init();

    while(1)
    {
    }
}
