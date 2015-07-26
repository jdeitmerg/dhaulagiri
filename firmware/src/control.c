#include "common.h"
#include "control.h"

void control_init()
{
    setbit(DDR_FAN, DDFAN);
    setbit(DDR_COMP, DDCOMP);
    //set the analog comparator AIN0 (PD6) to input. Should be around 4.1V
    //clearbit(DDRD, DDD6); //just leave it there, shouldn't be set in the
                            //first place

    //select AVCC as ADC Reference
    setbit(ADMUX, REFS0);
    //left adjust ADC results
    setbit(ADMUX, ADLAR);
    //50 - 200 kHz needed, 1MHz provided -> prescaler: 16 -> 62.5kHz
    clearbit(ADCSRA, ADPS0);
    clearbit(ADCSRA, ADPS1);
    setbit(ADCSRA, ADPS2);
}

uint32_t humidity(void)
{
    uint32_t counter = 0;
    /*discharge capacitor
     */
    //set pin as output, low, to discharge
    setbit(DDR_HUM, DDHUM);
    clearbit(PORT_HUM, PHUM);
    //the time this takes should be around 3*120nF*1000R = 360us
    _delay_ms(100);
    //set to input without pullup
    clearbit(DDR_HUM, DDHUM);
    clearbit(PORT_HUM, PHUM);

    //recharge starts immediately, connect PHUM to analog comparator (AC)
    setbit(SFIOR, ACME);    //enable multiplexer for AC
    //select PHUM (ADC2)
    clearbit(ADMUX, MUX0);
    setbit(ADMUX, MUX1);
    clearbit(ADMUX, MUX2);
    clearbit(ADMUX, MUX3);

    //wait until AC triggers
    while(1)
    {
        //_delay_us(500);
        //positive input (AIN0) is ~4.1V we're waiting for the increasing
        //capacitor voltage to cross that. So we're waiting for ACO to be
        //cleared
        if(!testbit(ACSR, ACO))
        {
            return(counter);
        }
        if(counter == UINT32_MAX)
            return 0;
        counter++;
    }
}

uint8_t ambient_temp(void)
{
    //select ADC3 from muxer
    setbit(ADMUX, MUX0);
    setbit(ADMUX, MUX1);
    clearbit(ADMUX, MUX2);
    clearbit(ADMUX, MUX3);

    //enable ADC
    setbit(ADCSRA, ADEN);
    //start ADC conversion
    setbit(ADCSRA, ADSC);
    //wait for conversion to finish
    loop_until_bit_is_set(ADCSRA, ADIF);
    //datasheet: "ADIF is cleared by writing a logical one to the flag"
    setbit(ADCSRA, ADIF);
    return(ADCH);
}

//Fan control routines
void start_fan(void)
{
    setbit(PORT_FAN, PFAN);
}

void stop_fan(void)
{
    clearbit(PORT_FAN, PFAN);
}

void toggle_fan(void)
{
    togglebit(PORT_FAN, PFAN);
}

//Compressor control routines
void start_comp(void)
{
    setbit(PORT_COMP, PCOMP);
}

void stop_comp(void)
{
    clearbit(PORT_COMP, PCOMP);
}

void toggle_comp(void)
{
    togglebit(PORT_COMP, PCOMP);
}
