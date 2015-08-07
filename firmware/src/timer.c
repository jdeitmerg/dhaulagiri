#include "common.h"
#include "timer.h"

//Linked list of registered timers
timer* list_head;   //pointer to first timer in linked list
timer last_timer;   //additional timer at the end of list, used to
                    //'run into' while looping over the list.

void timer_init(void)
{
    //We use timer1 in CTC (clear timer to zero when counter matches OCR1A)
    TCCR1B = (0<<WGM13) | (1<<WGM12);
    TCCR1A = (0<<WGM11) | (0<<WGM10);
    //timer1 Output Compare A Match Interrupt Enable
    setbit(TIMSK, OCIE1A);
}

int8_t find_free_id()
/*Get an id that is not yet in the linked list.
 *This is a brute force solution, it's not very fast. That's okay as long as
 *it's not used that often...
 */
{
    timer* t;
    int8_t id = 1;
    uint8_t found;

    if(list_head == NULL)   //list is empty
    {
        return 0;
    }

    while(1)    //we just assume there are less than 127 timers.
    {
        t = list_head;
        found = 0;
        while(t->next != NULL)
        {
            if(t->id == id)
            {
                found = 1;
                break;
            }
            t = t->next;
        }
        if(found == 0)
        {
            return id;
        }
        id++;
    }
}

int8_t register_timer(void (*fptr)(void), uint32_t ival)
/*Register a function which is to be called every $inval cpu cycles. To get
 *good performance, these intervals should all be powers of two (It should
 *also work well with non power of two values, but for some reason it doesn't).
 *
 *Return values:
 *  0-127   id of sucessfully configured new timer. Needed to deregister later.
 *  -1      malloc failed.
 *  -2      Reguested interval doesn't fit the resolution. If you want to
 *          register different timers with a big interval range, please make
 *          sure they have common divisors that are powers of two.
 */

/*size before making this dynamic:
    text    data     bss     dec     hex
    2462      54     151    2667     a6b
 *size after making this dynamic:
    text      data    bss    dec     hex
    3106       60      33    3199     c7f
 */

{
    timer* i;
    uint32_t smallest = UINT32_MAX; //smallest interval
    timer* new_timer = malloc(sizeof(timer));

    if (new_timer == NULL)
    {
        return -1;
    }
    //else

    new_timer->interval = ival;
    new_timer->funcptr = fptr;
    new_timer->next = &last_timer;
    new_timer-> id = find_free_id();
    if(list_head == NULL)   //First element in the list
    {
        list_head = new_timer;
    }
    else
    {
        //we don't have a list tail pointer, as that makes removing too
        //complicated, so we have to loop to find the last one.
        i = list_head;
        while(i->next != &last_timer);
        {
            i = i->next;
        }
        i->next = new_timer;
    }

    //loop through all registered timers to find smallest intervals
    i = list_head;
    while(i->next != NULL)
    {
        if(i->interval < smallest)
        {
            smallest = i->interval;
        }
        i = i->next;
    }


    /*Now, divide this interval by 1, 2, 3 (whatever is possible) and try to
     *divide the other intervals by that value. If it works, we have a common
     *divisor
     */
    uint32_t j = 1;
    uint32_t gcd;
    uint8_t found;
    while(1)
    {
        if(smallest%j == 0)
        {
            gcd = smallest/j;
            /*loop through all intervals and see if they're dividable by
             *the gcd we just choose
             */
            found = 1;
            i = list_head;
            while(i->next != NULL)
            {
                if(i->interval%gcd != 0)
                {
                    found = 0;
                    break;
                }
                i = i->next;
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

    /*Before we really do anything, check whether we cover the whole range
     *of registered intervals with that prescaler
     */
    if(gcd/presc > UINT16_MAX)
    {
        deregister_timer(new_timer->id);
        return -2;
    }
    //else

    //Now that we have the clock prescaler, scale all the timers down
    i = list_head;
    while(i->next != NULL)
    {
        i->downscaled = i->interval/presc;
        i->countdown = i->downscaled;
        i = i->next;
    }


    //The counter value at which we want to get an interrupt is gcd/presc
    OCR1A = (uint16_t)(gcd/presc);
    //Let's get the timer running!
    TCCR1B = (TCCR1B & ((1<<ICNC1) | (1<<ICES1) | (1<<WGM13) | (1<<WGM12)))
             | clk_sel;

    return new_timer->id;
}

void deregister_timer(int8_t id)
{
    timer* t;
    timer* prev = NULL; //this initialization costs 4 bytes of rom, but at
                        //least we don't get 'not initialized' warnings.

    t = list_head;

    while(t->next != NULL)
    {
        if(t->id == id)
        {
            if(t == list_head)  //we have to catch the first element
            {
                list_head = t->next;
            }
            else
            {
                prev->next = t->next;
            }
            free(t);
            //TODO: we might recalculate our timer prescaler now...
            return;
        }
        prev = t;
        t = t->next;
    }
}

ISR(TIMER1_COMPA_vect)
/*Everytime we get this interrupt, we have to check if any of the registered
 *timers wants to have it's function called. That's what the scaled countdown
 *of all the timers is for; we count them down each time this routine is
 *called. If the countdown becomes 0, we call the function and reset it to the
 *downscaled interval
 */
{
    //loop through all timers
    timer* i;
    i = list_head;
    while(i->next != NULL)
    {
        if(--(i->countdown) == 0)
        {
            i->funcptr();
            i->countdown = i->downscaled;
        }
        i = i->next;
    }
}
