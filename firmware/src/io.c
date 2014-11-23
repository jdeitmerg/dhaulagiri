#include <util/delay_basic.h>
#include "common.h"
#include "io.h"

uint8_t shiftr_state = 0x00;

void io_init(void)
{
    setbit(DDR_IOCLK, DDIOCLK);     //output
    setbit(DDR_IODAT, DDIODAT);     //output
    clearbit(DDR_IOKEY, DDIOKEY);   //input
    setbit(DDR_IOLED, DDIOLED);     //output
    setbit(DDR_IODIS0, DDIODIS0);   //output
    setbit(DDR_IODIS1, DDIODIS1);   //output

    clearbit(PORT_IOCLK, PIOCLK);   //low by default
}

inline void pulse_ioclk()
{
    //short pulse on CLK, "clocking occurs on the low-to-high-level transition"
    //assume CLK line is low!
    setbit(PORT_IOCLK, PIOCLK);
    _delay_loop_1(2);   //works without waiting, but pulsing at 1 MHz might
                        //be a bit of an overkill (we wait around 6 cycles)
    clearbit(PORT_IOCLK, PIOCLK);
}

inline void shiftr_pushb(uint8_t bit)
/*Push a single bit into the shift register of the IO panel (IC5)
 */
{
    if(bit)
    {
        setbit(PORT_IODAT, PIODAT);
    }
    else
    {
        clearbit(PORT_IODAT, PIODAT);
    }

    pulse_ioclk();
}

void shiftr_update()
/*Pushes shiftr_state into the shift register of the IO panel (IC5)
 */
{
    uint8_t i = 8;
    while(1)
    {
        i--;
        shiftr_pushb(testbit(shiftr_state, i));
        if(i == 0)
        {
            break;
        }
    }
}

void shiftr_setval(uint8_t value)
/*Pushes the given value into the shift register of the IO panel (IC5)
 */
{
    shiftr_state = value;
    shiftr_update();
}

void shiftr_setbit(uint8_t bit)
{
    setbit(shiftr_state, bit);
    shiftr_update();
}

void shiftr_clearbit(uint8_t bit)
{
    clearbit(shiftr_state, bit);
    shiftr_update();
}

/*There's a transistor between the AVR pin and the LEDC pin, so our signal
 *get's inverted.
 */

inline void set_LEDC()
{
    clearbit(PORT_IOLED, PIOLED);
}

inline void clear_LEDC()
{
    setbit(PORT_IOLED, PIOLED);
}

inline void toggle_LEDC()
{
    togglebit(PORT_IOLED, PIOLED);
}

/*There's a transistor between the AVR pin and the pin of the 7-segment
 *dispaly, so our signal get's inverted.
 */

inline void set_DIS0()
{
    clearbit(PORT_IODIS0, PIODIS0);
}

inline void clear_DIS0()
{
    setbit(PORT_IODIS0, PIODIS0);
}

inline void toggle_DIS0()
{
    togglebit(PORT_IODIS0, PIODIS0);
}

inline void set_DIS1()
{
    clearbit(PORT_IODIS1, PIODIS1);
}

inline void clear_DIS1()
{
    setbit(PORT_IODIS1, PIODIS1);
}

inline void toggle_DIS1()
{
    togglebit(PORT_IODIS1, PIODIS1);
}


uint8_t io_switches_raw(void)
/*Test the switches SW1 to SW4 and returns ored states*/
{
    clear_LEDC();
    clear_DIS0();
    clear_DIS1();
    uint8_t i, state = 0;
    for(i = 0; i < 4; ++i)
    {
        shiftr_setval(1<<i);    //SW1, ..., SW4
        _delay_loop_1(10);
        state |= testbit(PIN_IOKEY, PIOKEY) << i;
    }
    return(state);
}

void io_set_LEDs(uint8_t state)
/*Set the LEDs according to the given state (probably ored LED_*)
 */
{
    clear_DIS0();
    clear_DIS1();

    //It's save to power all LEDs at the same time (see file io_panel_PCB)
    clear_LEDC();
    shiftr_setval(~state);
    set_LEDC();
}
