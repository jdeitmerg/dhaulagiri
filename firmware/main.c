#define F_CPU 1000000UL
#define BAUD 2400UL

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <inttypes.h>

//fan connected to PC0
#define PORT_FAN PORTC
//pin number of fan in PORT_FAN
#define PFAN PC0
#define DDR_FAN DDRC
#define DDFAN DDC0

//compressor connecte to PD4
#define PORT_COMP PORTD
//pin number of compressor in PORT_COMP
#define PCOMP PD4
#define DDR_COMP DDRD
#define DDCOMP DDD4

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
    PORT_FAN |= 1 << PFAN;
}

void stop_fan(void)
{
    PORT_FAN &= ~(1 << PFAN);
}

void toggle_fan(void)
{
    PORT_FAN ^= 1 << PFAN;
}

//Compressor control routines
void start_comp(void)
{
    PORT_COMP |= 1 << PCOMP;
}

void stop_comp(void)
{
    PORT_COMP &= ~(1 << PCOMP);
}

void toggle_comp(void)
{
    PORT_COMP ^= 1 << PCOMP;
}

void init(void) {
    uart_init();
    //make compressor and fan outputs
    DDR_FAN = 1 << DDFAN;
    DDR_COMP = 1 << DDCOMP;
}

int main(void)
{
    init();

    char action;
    while(1)
    {
        action = (char) uart_getc();
        if(action == 'c')
            toggle_comp();
        else if(action == 'f')
            toggle_fan();
    }
}
