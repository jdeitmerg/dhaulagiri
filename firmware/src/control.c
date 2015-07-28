#include "common.h"
#include "control.h"

void control_init()
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
    //50 - 200 kHz needed, 1MHz provided -> prescaler: 4 -> 250kHz
    clearbit(ADCSRA, ADPS0);
    setbit(ADCSRA, ADPS1);
    clearbit(ADCSRA, ADPS2);

    //humidity sensor
    //excitation as outputs, high
    setbit(DDR_EXCIP, DDEXCIP);
    setbit(PORT_EXCIP, PEXCIP);
    setbit(DDR_EXCIM, DDEXCIM);
    setbit(PORT_EXCIM, PEXCIM);
    //Analog measurement pin as input
    clearbit(DDR_HUM, DDHUM);
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

ISR(TIMER0_OVF_vect)
{
    TCNT0 = 256-1000/8;  //We need 1000 interrupts/second, prescaler is 8
    togglebit(PORT_EXCIP, PEXCIP);
    togglebit(PORT_EXCIM, PEXCIM);
}

void excitation_start(void)
{
    TCNT0 = 256-1000/8;  //We need 1000 interrupts/second, prescaler is 8
    setbit(TCCR0, CS01); //Set prescaler to 8 and start timer0
    setbit(TIMSK, TOIE0);   //enable overflow interrupt

    //start first square
    clearbit(PORT_EXCIM, PEXCIM);
    setbit(PORT_EXCIP, PEXCIP);
    return;
}

void excitation_stop(void)
{
    clearbit(TCCR0, CS01);  //stop counting
    clearbit(TIMSK, TOIE0); //and disable interrupt
}


static uint8_t adc_singleshot()
{
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

static uint8_t temp_celsius(enum temp_sensor sensor, uint8_t rawval)
//convert raw ADC value to temperature in °C
{
    //Interpolation polynominal factors:
    //ambient first, cooling unit second
    static const double interpfactors[2][3] =
        {{0.000640967, 0.1431304871, -11.6516721212},
         {0.0004351878, 0.2011721783, -10.5522104343}};
    //Interpolation ranges (return lowest/highest if outside)
    static const uint8_t interpranges[2][2] =
        {{87, 238}, {70, 230}};
    double result;
    double inval;

    if(rawval < interpranges[sensor][0])
    {
        rawval = interpranges[sensor][0];
    }
    else if(rawval > interpranges[sensor][1])
    {
        rawval = interpranges[sensor][1];
    }

    inval = (double) rawval;

    result = inval*inval;
    result *= interpfactors[sensor][0];
    result += interpfactors[sensor][1]*inval;
    result += interpfactors[sensor][2];

    return((uint8_t) result);
}

uint8_t temp_measure(enum temp_sensor sensor)
{
    //ambient first, cooling unit second
    static const uint8_t adc_chnls[2] = {3, 1};
    uint8_t raw_adc;

    mux_select_ch(adc_chnls[sensor]);
    raw_adc = adc_singleshot();
    return(temp_celsius(sensor, raw_adc));
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
