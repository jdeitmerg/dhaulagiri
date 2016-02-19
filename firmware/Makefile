CC = avr-gcc
CC_ARGS = -Wall -O1 -lm
OBJCOPY = avr-objcopy
SRC_DIR = src
BUILD_DIR = bin
#Programmer device type
PG_TYPE = stk500
#Programmer serial device
PG_PORT = /dev/serial/by-id/usb-Silicon_Labs_myAVR_-_mySmartUSB_light_mySmartUSBlight*

#Serial device used for boot load programming
SER_DEV = /dev/serial/by-id/*USB-Serial_Controller*
#SER_DEV = /dev/serial/by-id/usb-Silicon_Labs_CP2102_USB_to_UART_Bridge_Controller_*
SER_BAUD = 9600

MMCU = atmega8

$(BUILD_DIR)/main.hex: $(BUILD_DIR)/main.elf
	$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/main.elf: $(SRC_DIR)/*.c $(SRC_DIR)/*.h
	@test -d $(BUILD_DIR) || (mkdir $(BUILD_DIR) && echo -e "Created $(BUILD_DIR)/ directory")
	@#it would be nice to have avr-gcc warn about declared/defined functions that aren't used anywhere. Seems like that's hard to achieve...
	@#$(^:%.h=) leaves out all .h files in the list of prequisites
	$(CC) $(CC_ARGS) -mmcu=$(MMCU) -o $@ $(^:%.h=)

burn: $(BUILD_DIR)/main.hex
	#avrdude -p m8 -c $(PG_TYPE) -P $(PG_PORT) -U flash:w:$(BUILD_DIR)/main.elf
	avr-FBoot -d $(SER_DEV) -b $(SER_BAUD) -p $<

clean:
	rm -f $(BUILD_DIR)/*
