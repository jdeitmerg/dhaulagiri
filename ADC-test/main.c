#define F_CPU 1000000UL
#define BAUD 9600UL
#include <util/setbaud.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

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

static void uart_init(void)
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

#define NUM_TIMINGS   8
uint16_t times[NUM_TIMINGS];
volatile uint8_t adc_done = 0;

ISR(ADC_vect)
{
    static uint8_t counter = 0;

    //start next conversion
    ADCSRA |= 1<<ADSC;

    times[counter++] = TCNT1;
    TCNT1 = 0;

    if (counter == NUM_TIMINGS)
    {
        //disable ADC
        ADCSRA = 0x00;
        adc_done = 1;
    }

    return;
}

static void adc_init(void)
{
    //select AVCC as reference, ADC0 from muxer, rigth adjust result
    ADMUX = 1<<REFS0;
    //clock prescaler of 16 -> 62.5kHz ADC clock
    ADCSRA = 1<<ADPS2;
    //enable ADC and its interrupt, NOT freerunning
    ADCSRA |= (1<<ADEN) | (1<<ADIE) | (0<<ADFR);
    return;
}

int main(void)
{
    uint8_t i;
    uart_init();
    adc_init();

    sei();

    //measured freerunning
    //without delay, the first interrupt comes after ~27 ADC cylces
    _delay_us(16*24);  //24 ADC cylces
    //result: no matter how long the delay, first conversion always takes
    //434µs ~ 27 ADC cylces.

    TCNT1 = 0;
    //enable Counter1, clock = clock_I/O (1MHz)
    TCCR1B = 1<<CS10;

    //start first ADC conversion
    ADCSRA |= 1<<ADSC;

    //wait for adc interrupt to fill array
    while(!adc_done){}

    for(i = 0; i < NUM_TIMINGS; ++i)
    {
        printf("\n%u", times[i]);
    }

    while(1){}
}

