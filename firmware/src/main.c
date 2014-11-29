#include <avr/io.h>
#include "io.h"
#include "common.h"

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


void uart_init(void)
{
    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;
    #if USE_2X
        UCSRA |= (1 << U2X);
    #else
        UCSRA &= ~(1 << U2X);
    #endif

    UCSRB |= (1<<TXEN);     //enable UART TX
    UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);   // asynchronous 8N1
    UCSRB |= (1<<RXEN);     //enable UART RX
}

uint8_t uart_getc(void) {
    while (!(UCSRA & (1<<RXC)))
    {
    }
    return UDR;
}

void uart_putc(unsigned char c)
{
    while (!(UCSRA & (1<<UDRE)))
    {
    }
    UDR = c;
}

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
    //initialize input/output panel
    io_init();
}

int main(void)
{
    init();

    io_test();

    while(1)
    {
        //simple blink pattern
        io_set_LEDs(LED_ONOFF);
        _delay_ms(500);
        io_set_LEDs(LED_WATER);
        _delay_ms(500);
        io_set_LEDs(LED_CONT);
        _delay_ms(500);

        io_set_LEDs(0);
        _delay_ms(500);
        io_set_LEDs(LED_ONOFF | LED_WATER | LED_CONT);
        _delay_ms(500);
        io_set_LEDs(0);
        _delay_ms(500);
    }

}
