#include <avr/io.h>
#include "io.h"
#include "common.h"
#include "timer.h"
#include "uart.h"
#include "control.h"

void init(void) {
    uart_init();

    control_init();

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
        printf("%lu\n", read_humidity());
        _delay_ms(300);
    }
}
