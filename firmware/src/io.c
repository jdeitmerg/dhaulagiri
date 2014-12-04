#include <util/delay_basic.h>
#include "common.h"
#include "io.h"
#include "timer.h"

//State of outputs
uint8_t LEDs_state = 0xFF;
uint8_t DIS0_state = 0xFF;
uint8_t DIS1_state = 0xFF;


//Bitpatterns for 7 segment digits
uint8_t dis_digits[] = {
    ~0x3F,   //'0'
    ~0x06,   //'1'
    ~0x5B,   //'2'
    ~0x4F,   //'3'
    ~0x66,   //'4'
    ~0x6D,   //'5'
    ~0x7D,   //'6'
    ~0x07,   //'7'
    ~0x7F,   //'8'
    ~0x6F    //'9'
};

//Bitpatterns for 7 segment letters
uint8_t dis_letters[] = {
    ~0x77,  //'A'
    ~0x7C,  //'B'
    ~0x39,  //'C'
    ~0x5E,  //'D'
    ~0x79,  //'E'
    ~0x71,  //'F'
    ~0x6F,  //'G' like '9'
    ~0x76,  //'H' like 'X'
    ~0x10,  //'I'
    ~0x1F,  //'J'
    ~0x78,  //'K'
    ~0x38,  //'L'
    ~0x37,  //'M' cripple
    ~0x54,  //'N'
    ~0x3F,  //'O' like '0'
    ~0x73,  //'P'
    ~0x67,  //'Q'
    ~0x50,  //'R'
    ~0x6D,  //'S' like '5'
    ~0x31,  //'T'
    ~0x3E,  //'U'
    ~0x1C,  //'V'
    ~0x3C,  //'W' cripple
    ~0x76,  //'X' like 'H'
    ~0x70,  //'Y'
    ~0x5B   //'Z' like '2'
};

uint8_t dischar(char letter)
/*Return 7 segment state of any of the characters we have defined above
 */
{
    if(letter >= 'a' && letter <= 'z')
    {
        //convert to upper
        letter -= 'a'-'A';
    }
    if(letter >= 'A' && letter <= 'Z')
    {
        return dis_letters[letter-'A'];
    }
    else if (letter >= '0' && letter <= '9')
    {
        return dis_digits[letter-'0'];
    }
    else
    {
        return (uint8_t)~0x00;
    }
}

inline uint8_t disdigit(uint8_t digit)
{
    return dis_digits[digit];
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

void shiftr_setval(uint8_t value)
/*Pushes the given value into the shift register of the IO panel (IC5)
 *We don't do any abstraction between this and setting 7seg / LED patterns
 */
{
    uint8_t i = 8;
    while(1)
    {
        i--;
        shiftr_pushb(testbit(value, i));
        if(i == 0)
        {
            break;
        }
    }
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
        shiftr_setval(~(1<<i));    //SW1, ..., SW4
        state |= testbit(~PIN_IOKEY, PIOKEY) << i;
    }
    return(state);
}

inline void switch_handler(void)
{
    //we don't need to do this very often
    //Maybe we could share this time slot with the LEDs then...
    static uint8_t delay_counter = 0;
    if(++delay_counter < 5)
    {
        return;
    }
    //else
    delay_counter = 0;

    static uint8_t old_state;   //For checking whether switches were pressed
                                //before

    //For now, we'll just have all the buttons increment a digit on DIS0.
    static uint8_t countup = 0;

    uint8_t new_state;
    new_state = io_switches_raw();
    //only do something when the switch wasn't pressed before
    uint8_t switches = (old_state ^ new_state) & new_state;
    old_state = new_state;
    if(switches & SW_ONOFF)
    {
        DIS0_state = disdigit(countup++ %10);
    }
    if(switches & SW_DOWN)
    {
        DIS0_state = disdigit(countup++ %10);
    }
    if(switches & SW_UP)
    {
        DIS0_state = disdigit(countup++ %10);
    }
    if(switches & SW_CONT)
    {
        DIS0_state = disdigit(countup++ %10);
    }
}

void disp_cycle(void)
/*This routine is responsible for cycling through the different outputs, as we
 *can only do output on one of {LEDs, DIS0, DIS1} or input of the switches at
 *the same time.
 */
{
    static volatile uint8_t curr_state;  //current state

    //turn everything off
    clear_LEDC();
    clear_DIS0();
    clear_DIS1();
    //turn on depending on current state
    switch(curr_state)
    {
        case 0:
            shiftr_setval(LEDs_state);
            set_LEDC();
            break;
        case 1:
            shiftr_setval(DIS0_state);
            set_DIS0();
            break;
        case 2:
            shiftr_setval(DIS1_state);
            set_DIS1();
            break;
        case 3:
            switch_handler();
            break;
    }

    if(++curr_state == 4)
    {
        curr_state = 0;
    }
}

void io_set_LEDs(uint8_t state)
/*Set the LEDs according to the given state (probably ored LED_*)
 */
{
    LEDs_state = state;
}

void ticker_pr(char str[])
/*Dispaly the supplied string on the two 7 segments once. Blocking!
 */
{
    //We give only 3ms to the delay func, but the actual delay is much longer

    uint8_t sp0 = 1;
    uint8_t sp1 = 0;
    //At first, display only first char on right 7 segment
    DIS1_state = 0xFF;  //nothing on left display
    DIS0_state = dischar(str[0]);
    _delay_ms(500);
    while(str[sp1] != 0)
    {
        DIS0_state = dischar(str[sp0]);
        DIS1_state = dischar(str[sp1]);
        _delay_ms(500);
        sp0++;
        sp1++;
    }
    //flush the display
    DIS0_state = 0xFF;
    DIS1_state = 0xFF;
    _delay_ms(500);
}

void io_init(void)
{
    setbit(DDR_IOCLK, DDIOCLK);     //output
    setbit(DDR_IODAT, DDIODAT);     //output
    clearbit(DDR_IOKEY, DDIOKEY);   //input
    setbit(DDR_IOLED, DDIOLED);     //output
    setbit(DDR_IODIS0, DDIODIS0);   //output
    setbit(DDR_IODIS1, DDIODIS1);   //output

    clearbit(PORT_IOCLK, PIOCLK);   //low by default

    clear_LEDC();
    clear_DIS0();
    clear_DIS1();

    register_timer(&disp_cycle, 1024);
}
