#include "common.h"
#include "control.h"
#include <math.h>

#define NUM_HUM_READS 32

volatile uint8_t hum_readings[NUM_HUM_READS];
volatile uint8_t humread_active = 0;
volatile uint8_t cycle_step = 0;

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

ISR(ADC_vect)
{
    static volatile uint8_t count = 0;

    switch(cycle_step)
    {
        //assume we get the interrupt of the conversion started in previous
        //step...
        case 2:
            hum_readings[count] = ADCH;
            break;
        case 0: //conversion was started in cycle 4, then cycle was reset
            //invert value if excitation voltage is inverted
            hum_readings[count] = 255-ADCH;
            break;
        default:
            //waste some bytes...
            //printf("\nError: ADC interrupt fired at the wrong time!");
            break;
    }
    count++;

    if(count == NUM_HUM_READS)
    {
        count = 0;
        humread_active = 0;
        //disable adc interrupt
        clearbit(ADCSRA, ADIE);
    }
    return;
}

ISR(TIMER0_OVF_vect)
{
#ifndef EXCI_ONE_PORT
#error TIMER0 overflow handler implemented only for EXCI pins on same port!
#endif

    volatile uint8_t ioreg;

    TCNT0 = 256-1e6/8/2000;  //We need 2000 interrupts/second, cpu freq is
                             //1Mhz, prescaler is 8
    cycle_step++;
    switch(cycle_step)
    {
        case 1:
            //t=0, raising edge
            ioreg = PORT_EXCIM;
            setbit(ioreg, PEXCIP);
            clearbit(ioreg, PEXCIM);
            PORT_EXCIM = ioreg;
            break;
        case 2:
            //t=T/4, sample high
            setbit(ADCSRA, ADSC);
            break;
        case 3:
            //t=T/2, falling edge
            ioreg = PORT_EXCIM;
            clearbit(ioreg, PEXCIP);
            setbit(ioreg, PEXCIM);
            PORT_EXCIM = ioreg;
            break;
        case 4:
            //t=3T/4, sample low
            setbit(ADCSRA, ADSC);
            cycle_step = 0;
    }
    return;
}

static void hum_start(void)
{
    mux_select_ch(HUM_CHNL);
    //enable ADC interrupt
    setbit(ADCSRA, ADIE);

    TCNT0 = 256-1e6/8/2000; //We need 2000 interrupts/second, cpu freq is
                            //1Mhz, prescaler is 8
//    setbit(TCCR0, CS01);    //Set prescaler to 8 and start timer0
    TCCR0 = 1<<CS01;
    setbit(TIMSK, TOIE0);   //enable overflow interrupt
    return;
}

static void hum_stop(void)
{
    //clearbit(TCCR0, CS01);  //stop counting
    TCCR0 = 0x00;
    clearbit(TIMSK, TOIE0); //and disable interrupt
    return;
}

static uint8_t hum_from_imp(double imp, uint8_t temperature)
//Humidity in percent from impedance
{
    //constants from datasheet, factor 100 to get percent
    const double constants[6][4] = {
        {100*2.795953, -2346.986, 0.1299848, 4.067295},
        {100*10.83324, -2368.839, 28.18587, 3.016073},
        {100*29.29039, -2426.034, 49399.25, 1.993527},
        {100*2.79711, -7813.2, 100848.4, 9.437572},
        {100*5.729313, -5357.106, 19934420, 4.242907},
        {100*3.158756, -6368.854, 1247857, 5.748199}};
    uint8_t range;  //impedance range, for selecting constant
    double temp;

    if(imp <= .005)
        range = 5;
    else if(imp <= .05)
        range = 4;
    else if(imp <= 5)
        range = 3;
    else if(imp <= 12)
        range = 2;
    else if(imp <= 30)
        range = 1;
    else
        range = 0;

    //temperature in fahrenheit
    temp = 1.8*temperature+32.;

    //UPS-500 Equation
    temp += 459.7;
    temp = 1/(constants[range][3]+constants[range][1]/temp);
    temp = pow(imp*constants[range][2], temp);
    temp *= constants[range][0];

    return((uint8_t) temp);
}

static double imp_from_adc(uint8_t adcval)
//calculate impedance (im meg-ohms) of humidity sensor based on raw ADC value
{
    if(adcval == 0)
        return(0);
    if(adcval == 255)
        return(40);    //made up, probably never gonna be that dry
    //R38=47kOhms voltage divider
    return(.047/(255./adcval-1.));
}

uint8_t hum_measure(void)
{
    uint16_t sum = 0;
    uint8_t i;
    uint8_t temperature;
    double impedance;

    hum_start();
    humread_active = 1;
    while(humread_active){}
    hum_stop();

    //hum_readings should now be filled with values, calculate average
    for(i = 0; i < NUM_HUM_READS; ++i)
    {
        sum += hum_readings[i];
    }

    temperature = temp_measure(ambient);
    impedance = imp_from_adc((uint8_t) (sum/NUM_HUM_READS));

    return(hum_from_imp(impedance, temperature));
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
