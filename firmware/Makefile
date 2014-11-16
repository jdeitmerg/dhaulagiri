CC = avr-gcc
CC_ARGS = -Wall
BUILD_DIR = bin
#Programmer device type
PG_TYPE = stk500
#Programmer serial device
PG_PORT = /dev/ttyUSB0

compile: main.c
	@test -d $(BUILD_DIR) || (mkdir $(BUILD_DIR) && echo -e "Created $(BUILD_DIR)/ directory")
	$(CC) $(CC_ARGS) -mmcu=atmega8 -O1 -o $(BUILD_DIR)/main main.c

burn: compile
	sudo avrdude -p m8 -c $(PG_TYPE) -P $(PG_PORT) -U flash:w:main

check:
	sudo avrdude -p m8 -c $(PG_TYPE) -P $(PG_PORT)

all: compile