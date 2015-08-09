CC = avr-gcc
CC_ARGS = -Wall -O1 -lm
OBJCOPY = avr-objcopy
SRC_DIR = src
BUILD_DIR = bin
#Programmer device type
#PG_TYPE = stk500
#Programmer serial device
#PG_PORT = /dev/ttyUSB0
#AVR mmcu

#Serial device used for boot load programming
SER_DEV = /dev/serial/by-id/*USB-Serial_Controller*
SER_BAUD = 9600
MMCU = atmega8

$(BUILD_DIR)/main.hex: $(BUILD_DIR)/main.elf
	$(OBJCOPY) -O ihex $(BUILD_DIR)/main.elf $(BUILD_DIR)/main.hex

$(BUILD_DIR)/main.elf: $(SRC_DIR)/main.c $(SRC_DIR)/io.c $(SRC_DIR)/io.h $(SRC_DIR)/common.h $(SRC_DIR)/timer.c $(SRC_DIR)/timer.h $(SRC_DIR)/uart.h $(SRC_DIR)/uart.c $(SRC_DIR)/control.h $(SRC_DIR)/control.c
	@test -d $(BUILD_DIR) || (mkdir $(BUILD_DIR) && echo -e "Created $(BUILD_DIR)/ directory")
	@#it would be nice to have avr-gcc warn about declared/defined functions that aren't used anywhere. Seems like that's hard to achieve...
	$(CC) $(CC_ARGS) -mmcu=$(MMCU) -o $(BUILD_DIR)/main.elf $(SRC_DIR)/main.c $(SRC_DIR)/io.c $(SRC_DIR)/timer.c $(SRC_DIR)/uart.c $(SRC_DIR)/control.c

burn: $(BUILD_DIR)/main.hex
	#avrdude -p m8 -c $(PG_TYPE) -P $(PG_PORT) -U flash:w:$(BUILD_DIR)/main.elf
	avr-FBoot -d $(SER_DEV) -b $(SER_BAUD) -p -v $(BUILD_DIR)/main.hex
