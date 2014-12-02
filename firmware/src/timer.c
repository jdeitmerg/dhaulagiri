#include "common.h"
#include "timer.h"

timer timers[10];   //Note: you can only register a maximum of 9 timers!

void timer_init(void)
{
    //We use timer1 in CTC (clear timer to zero when counter matches OCR1A)
    TCCR1B = (1<<WGM13) | (1<<WGM12);
    TCCR1A = (0<<WGM11) | (0<<WGM10);
    //timer1 Output Compare A Match Interrupt Enable
    TIMSK |= 1<<OCIE1A;
}

void register_timer(void (*fptr)(void), uint32_t ival)
{
    uint8_t i = 0;
    uint32_t smallest = 0xFFFFFFFF; //smallest interval

    //loop through all registered timers to find smallest intervals
    while(timers[i].funcptr != 0)
    {
        if(timers[i].interval < smallest)
        {
            smallest = timers[i].interval;
        }
        i++;
    }

    /*Now, divide this interval by 1, 2, 3 (whatever is possible) and try to
     *divide the other intervals by that value. If it works, we have a common
     *divisor
     */
    uint32_t j = 1;
    uint32_t gcd;
    uint8_t found = 0;
    while(1)
    {
        if(smallest%j == 0)
        {
            gcd = smallest/j;
            /*loop through all intervals and see if they're dividable by
             *divisor
             */
            i = 0;
            found = 1;
            while(timers[i].funcptr != 0)
            {
                if(timers[i].interval%gcd != 0)
                {
                    found = 0;
                    break;
                }
                i++;
            }
            //Check if we broke. If not, we found our gcd!
            if(found)
            {
                break;
            }
        }
        j++;
    }

    //Depending on our gcd, we can intelligently choose a clock prescaler.
    uint8_t clk_sel;
    uint16_t presc;
    if(gcd%1024 == 0)
    {
        presc = 1024;
        clk_sel = (1<<CS12) | (0<<CS11) | (1<<CS10);
    }
    else if(gcd%256 == 0)
    {
        presc = 256;
        clk_sel = (1<<CS12) | (0<<CS11) | (0<<CS10);
    }
    else if(gcd%64 == 0)
    {
        presc = 64;
        clk_sel = (0<<CS12) | (1<<CS11) | (1<<CS10);
    }
    else if(gcd%8 == 0)
    {
        presc = 8;
        clk_sel = (0<<CS12) | (1<<CS11) | (0<<CS10);
    }
    else
    {
        presc = 1;
        clk_sel = (0<<CS12) | (0<<CS11) | (1<<CS10);
    }

    //Now that we have the clock prescaler, scale all the timers down
    i = 0;
    while(timers[i].funcptr != 0)
    {
        timers[i].downscaled = timers[i].interval/presc;
        i++;
    }

    //The counter value at which we want to get an interrupt is gcd/presc
    OCR1A = (uint16_t)(gcd/presc);
    //Let's get the timer running!
    TCCR1B = (TCCR1B & ((1<<ICNC1) | (1<<ICES1) | (1<<WGM13) | (1<<WGM12)))
             | clk_sel;
}

ISR(TIMER1_COMPA_vect)
{

}
