CC = avr-gcc
CC_ARGS = -Wall -O1
SRC_DIR = src
BUILD_DIR = bin
#Programmer device type
PG_TYPE = stk500
#Programmer serial device
PG_PORT = /dev/ttyUSB0
#AVR mmcu
MMCU = atmega8

compile: $(SRC_DIR)/main.c $(SRC_DIR)/io.c $(SRC_DIR)/io.h $(SRC_DIR)/common.h $(SRC_DIR)/timer.c $(SRC_DIR)/timer.h $(SRC_DIR)/uart.h $(SRC_DIR)/uart.c $(SRC_DIR)/control.h $(SRC_DIR)/control.c
	@test -d $(BUILD_DIR) || (mkdir $(BUILD_DIR) && echo -e "Created $(BUILD_DIR)/ directory")
	@#it would be nice to have avr-gcc warn about declared/defined functions that aren't used anywhere. Seems like that's hard to achieve...
	$(CC) $(CC_ARGS) -mmcu=$(MMCU) -o $(BUILD_DIR)/main $(SRC_DIR)/main.c $(SRC_DIR)/io.c $(SRC_DIR)/timer.c $(SRC_DIR)/uart.c $(SRC_DIR)/control.c

burn: compile
	sudo avrdude -p m8 -c $(PG_TYPE) -P $(PG_PORT) -U flash:w:$(BUILD_DIR)/main

check:
	sudo avrdude -p m8 -c $(PG_TYPE) -P $(PG_PORT)

all: compile