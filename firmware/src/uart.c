#include "common.h" 
#include "uart.h"

//Like the glibc example
int uart_putchar(char c, FILE* stream)
{
    if (c == '\n')
    {
        uart_putchar('\r', stream);
    }
    loop_until_bit_is_set(UCSRA, UDRE);
    UDR = c;
    return 0;
}

int uart_getchar(FILE *stream) {
    loop_until_bit_is_set(UCSRA, RXC);
    return UDR;
}

static FILE uart_stream = FDEV_SETUP_STREAM(uart_putchar, uart_getchar,
                                             _FDEV_SETUP_RW);

void uart_init(void)
{
    //also pretty much what the glibc pages tell you to do
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

    stdout = &uart_stream;
    stdin = &uart_stream;
}
