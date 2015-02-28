#include <avr/io.h>
#include "io.h"
#include "common.h"
#include "timer.h"
#include "uart.h"
#include "control.h"

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
