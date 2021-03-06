###############################################################################
# Makefile for the project lf
###############################################################################

## General Flags
PROJECT = lf
MCU = atmega16
TARGET = lf.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=lf.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Library Directories
LIBDIRS = -L"C:\Users\AND\Documents\gg\embeddedsociety\line follower\code\new_version\lf\." 

## Objects that must be built in order to link
OBJECTS = lf.o pwm.o QTR.o uart.o timer.o scheduler.o pid.o communication_handler.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) lf.hex lf.eep lf.lss## Compile
lf.o: ../lf.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

pwm.o: ../pwm.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

QTR.o: ../QTR.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uart.o: ../uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

timer.o: ../timer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

scheduler.o: ../scheduler.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

pid.o: ../pid.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

communication_handler.o: ../communication_handler.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) lf.elf dep/* lf.hex lf.eep lf.lss lf.map


## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

