#include "common.h"
#include "control.h"

void control_init()
{
	setbit(DDR_FAN, DDFAN);
    setbit(DDR_COMP, DDCOMP);
    //set the analog comparator AIN0 (PD6) to input. Should be around 4.1V
    clearbit(DDRD, DDD6);
    //select negative input of analog comparator from ADC multiplexer
}

uint32_t read_humidity(void)
{
	uint32_t counter = 0;
	/*discharge capacitor
	 */
	//set pin as output, low, to discharge
	setbit(DDR_HUM, DDHUM);
	clearbit(PORT_HUM, PHUM);
	//the time this takes should be around 3*120nF*1000R = 360us
	_delay_ms(100);
	//set to input with pullup
	clearbit(DDR_HUM, DDHUM);
	setbit(PORT_HUM, PHUM);

	//recharge starts immediately, connect PHUM to analog comparator (AC)
	setbit(SFIOR, ACME);	//enable multiplexer for AC
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
