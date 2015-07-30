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

/*ambient temperature sensor
 */
#define PORT_ATS    PORTC
#define PATS        PC3
#define DDR_ATS     DDRC
#define DDATS       DDC3

/*humidity sensor
 */
#define PORT_HUM    PORTC
#define PHUM        PC2
#define DDR_HUM     DDRC
#define DDHUM       DDC2
#define HUM_CHNL    2

//humidity sensor excitation +
#define PORT_EXCIP  PORTB
#define PEXCIP      PB1
#define DDR_EXCIP   DDRB
#define DDEXCIP     DDB1

//humidity sensor excitation -
#define PORT_EXCIM  PORTB
#define PEXCIM      PB2
#define DDR_EXCIM   DDRB
#define DDEXCIM     DDB2
//humidity sensor excitation on the same port (PORTB)
#define EXCI_ONE_PORT

/*cooling unit temperature sensor
 */
#define PORT_CTS    PORTC
#define PCTS        PC1
#define DDR_CTS     DDRC
#define DDCTS       DDC1

/*water full sensor
 */
#define PORT_FULL   PORTB
#define PFULL       PB0
#define DDR_FULL    DDRB
#define DDFULL      DDB0

enum temp_sensor
{
    ambient = 0,
    cool_unit = 1
};

void control_init();

void excitation_start(void);
void excitation_stop(void);

uint8_t temp_measure(enum temp_sensor);
//Fan control routines
void start_fan(void);
void stop_fan(void);
void toggle_fan(void);
//Compressor control routines
void start_comp(void);
void stop_comp(void);
void toggle_comp(void);

#endif
