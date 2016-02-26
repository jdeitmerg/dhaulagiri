#include "common.h"
#include "control.h"
#include <math.h>

static uint8_t adc_singleshot()
{
    //start ADC conversion
    setbit(ADCSRA, ADSC);
    //wait for conversion to finish
    loop_until_bit_is_set(ADCSRA, ADIF);
    //datasheet: "ADIF is cleared by writing a logical one to the flag"
    setbit(ADCSRA, ADIF);
    return(ADCH);
}

void control_init(void)
{
    setbit(DDR_FAN, DDFAN);
    setbit(DDR_COMP, DDCOMP);
    //set the analog comparator AIN0 (PD6) to input. Should be around 4.1V
    //clearbit(DDRD, DDD6); //just leave it there, shouldn't be set in the
                            //first place

    //ADC setup
    //select AVCC as ADC Reference
    setbit(ADMUX, REFS0);
    //left adjust ADC results
    setbit(ADMUX, ADLAR);
    //50 - 200 kHz needed, 1MHz provided -> prescaler: 16 -> 62.5kHz
    clearbit(ADCSRA, ADPS0);
    clearbit(ADCSRA, ADPS1);
    setbit(ADCSRA, ADPS2);
    clearbit(ADCSRA, ADFR);
    //enable ADC and start first conversion, so subsequent ones take only 13
    //cycles
    setbit(ADCSRA, ADEN);
    adc_singleshot();

    //water full sensor
    clearbit(DDR_FULL, DDFULL);
    setbit(PORT_FULL, PFULL);   //enable pullup
}

static void mux_select_ch(uint8_t chnl)
{
    //not nice and fast, but independent of register positions
    //clear all mux bits in ADMUX
    ADMUX &= ~(MUX0|MUX1|MUX2|MUX3);
    //Set only those which are 1 in argument
    if(chnl & 0x01)
    {
        setbit(ADMUX, MUX0);
    }
    if(chnl & 0x02)
    {
        setbit(ADMUX, MUX1);
    }
    if(chnl & 0x04)
    {
        setbit(ADMUX, MUX2);
    }
    if(chnl & 0x08)
    {
        setbit(ADMUX, MUX3);
    }

    return;
}

static uint8_t temp_celsius(uint8_t rawval)
//convert raw ADC value to temperature in °C
{
    double result;
    double inval;

    //Keep value in interpolation ranges
    if(rawval < 70)
    {
        rawval = 70;
    }
    else if(rawval > 230)
    {
        rawval = 230;
    }

    inval = (double) rawval;

    //Polynominal interpolation
    result = inval*inval;
    result *= 0.0004351878;
    result += 0.2011721783*inval;
    result += -10.5522104343;

    return((uint8_t) result);
}

uint8_t temp_measure(void)
{
    uint8_t raw_adc;

    mux_select_ch(1);
    raw_adc = adc_singleshot();
    return(temp_celsius(raw_adc));
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

uint8_t water_full(void)
{
    return(testbit(PIN_FULL, PFULL));
}

