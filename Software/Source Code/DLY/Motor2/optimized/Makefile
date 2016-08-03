###############################################################################
# Makefile for the project Motor2
###############################################################################

## General Flags
PROJECT = Motor2
MCU = atmega2560
TARGET = Motor2.elf
CC = avr-gcc.exe

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -x  c++                                        -DF_CPU=16000000UL -O3 -fsigned-char
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += 


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I"D:\protel projects\sumo\19-06-2007 reroute\Source Code\kenmodified\Motor2\..\Global" -I"D:\protel projects\sumo\19-06-2007 reroute\Source Code\kenmodified\Motor2\." 

## Libraries
LIBS = -lc 

## Objects that must be built in order to link
OBJECTS = main.o sumo_ports.o adpcm.o IntTimer.o Motor2.o sumo_adc.o sumo_irsensor.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) Motor2.hex Motor2.eep size

## Compile
main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sumo_ports.o: ../sumo_ports.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

adpcm.o: ../adpcm.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

IntTimer.o: ../IntTimer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

Motor2.o: ../Motor2.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sumo_adc.o: ../sumo_adc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sumo_irsensor.o: ../sumo_irsensor.c
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

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) Motor2.elf dep/* Motor2.hex Motor2.eep

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

