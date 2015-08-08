#include <avr/io.h>
#include "io.h"
#include "common.h"
#include "timer.h"
#include "uart.h"
#include "control.h"

//visible in all modules as declared in common.h
uint8_t ref_hum = 25;
enum statev state = ok;

#define REF_TDIFF   8   //keep cooling unit x°C below ambient temperature

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

    uint8_t hum;
    int8_t tempdiff;    //temperature diff of air and cooling unit

    while(1)
    {
        hum = hum_measure();
        switch(state)
        {
        case waterfull:
            io_set_LEDs(LED_ONOFF | LED_WATER);
            break;
        case ok:
            io_set_LEDs(LED_ONOFF);
            if(hum > ref_hum)
            {
                start_fan();
                tempdiff = temp_measure(ambient)-temp_measure(cool_unit);
                if(tempdiff < REF_TDIFF)
                {
                    start_comp();
                }
                else
                {
                    stop_comp();
                }
            }
            else if(hum < ref_hum-ref_hum_var)
            {
                stop_comp();
                stop_fan();
            }
            if(water_full())
            {
                stop_comp();
                stop_fan();
                state = waterfull;
            }
        }
        _delay_ms(1000);
    }
}
