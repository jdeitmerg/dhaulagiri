#ifndef CONTROL_H
#define CONTROL_H

/*fan connection
 */
//connected to PC0
#define PORT_FAN    PORTC
//pin number of fan in PORT_FAN
#define PFAN        PC0
#define DDR_FAN     DDRC
#define DDFAN       DDC0

/*compressor connection
 */
//connected to PD4
#define PORT_COMP   PORTD
//pin number of compressor in PORT_COMP
#define PCOMP       PD4
#define DDR_COMP    DDRD
#define DDCOMP      DDD4

/*cooling unit temperature sensor
 */
#define PORT_CTS    PORTC
#define PCTS        PC1
#define DDR_CTS     DDRC
#define DDCTS       DDC1

/*water full sensor
 */
#define PORT_FULL   PORTB
#define PIN_FULL    PINB
#define PFULL       PB0
#define DDR_FULL    DDRB
#define DDFULL      DDB0

enum temp_sensor
{
    ambient = 0,
    cool_unit = 1
};

void control_init(void);

uint8_t temp_measure(enum temp_sensor);
//Fan control routines
void start_fan(void);
void stop_fan(void);
void toggle_fan(void);
//Compressor control routines
void start_comp(void);
void stop_comp(void);
void toggle_comp(void);

uint8_t water_full(void);

#endif
