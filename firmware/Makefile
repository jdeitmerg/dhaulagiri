CC = avr-gcc
CC_ARGS = -Wall
SRC_DIR = src
BUILD_DIR = bin
#Programmer device type
PG_TYPE = stk500
#Programmer serial device
PG_PORT = /dev/ttyUSB0
#AVR mmcu
MMCU = atmega8

compile: $(SRC_DIR)/main.c $(SRC_DIR)/io.c $(SRC_DIR)/io.h $(SRC_DIR)/common.h
	@test -d $(BUILD_DIR) || (mkdir $(BUILD_DIR) && echo -e "Created $(BUILD_DIR)/ directory")
	$(CC) $(CC_ARGS) -mmcu=$(MMCU) -O1 -o $(BUILD_DIR)/main $(SRC_DIR)/main.c $(SRC_DIR)/io.c

burn: compile
	sudo avrdude -p m8 -c $(PG_TYPE) -P $(PG_PORT) -U flash:w:$(BUILD_DIR)/main

check:
	sudo avrdude -p m8 -c $(PG_TYPE) -P $(PG_PORT)

all: compile