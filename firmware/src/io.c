#include <avr/io.h>
#include <stdint.h>
#include <util/delay_basic.h>
#include "bitio.h"
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
    //short pulse on CLK
    setbit(PORT_IOCLK, PIOCLK);
    _delay_loop_1(10);  //we can make this value smaller as soon as we know
                        //this works.
    clearbit(PORT_IOCLK, PIOCLK);
}

void shiftr_update()
/*Pushes shiftr_state into the shift register of the IO panel (IC5)
 */
{
    uint8_t i;
    for(i = 0; i < 8; ++i)
    {
        shiftr_pushb((1 << i) & shiftr_state);
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
