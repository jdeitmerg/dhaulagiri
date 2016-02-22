#include <avr/io.h>
#include "io.h"
#include "common.h"
#include "timer.h"
#include "uart.h"
#include "control.h"
#include "dht.h"

//visible in all modules as declared in common.h
uint8_t ref_hum = 25;
enum statev state = ok;

//keep cooling unit between these two values (°C) below ambient temperature
#define REF_TDIFF_L     7
#define REF_TDIFF_H     10

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

    int8_t hum;
    float hum_f;

    int8_t ambient_temp;
    float ambient_temp_f;

    int8_t tempdiff;    //temperature diff of air and cooling unit

    while(1)
    {
        if(dht_gettemperaturehumidity(&ambient_temp_f, &hum_f))
        {
            hum = 30;
            ambient_temp = 21;
        }
        hum = hum_f;
        ambient_temp = ambient_temp_f;

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
                tempdiff = ambient_temp-temp_measure(cool_unit);
                if(tempdiff < REF_TDIFF_L)
                {
                    start_comp();
                }
                else if(tempdiff > REF_TDIFF_H)
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
            break;
        case off:
            io_set_LEDs(0);
            io_print_nbr(100);  //clear display
            stop_comp();
            stop_fan();
        }
        _delay_ms(300);
    }
}
