main: main.c
	avr-gcc -Wall -O1 -mmcu=atmega8 -o main main.c
burn: main
	avrdude -p m8 -c stk500 -P /dev/ttyUSB0 -U flash:w:main

all: main

