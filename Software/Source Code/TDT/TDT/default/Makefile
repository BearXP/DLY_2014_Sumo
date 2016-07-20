###############################################################################
# Makefile for the project TDT
###############################################################################

## General Flags
PROJECT = TDT
MCU = atmega2560
TARGET = TDT.elf
CC = avr-gcc.exe

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2  -x c++      -DF_CPU=16000000UL -O0 -fsigned-char
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
INCLUDES = -I"G:\VBS Workgroups\Engineering\TDT\Sumo\Software\Source Code\TDT\TDT\src" 

## Objects that must be built in order to link
OBJECTS = sumo_dotmatrix.o sumo_ports.o sumo_rs232.o TDT.o dynamic_alloc.o sumo_timer.o sumo_adc.o sumo_irsensor.o sumo_line.o sumo_motor_fs.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) TDT.hex TDT.eep size

## Compile
sumo_dotmatrix.o: ../src/sumo_dotmatrix.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sumo_ports.o: ../src/sumo_ports.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sumo_rs232.o: ../src/sumo_rs232.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

TDT.o: ../src/TDT.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

dynamic_alloc.o: ../src/dynamic_alloc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sumo_timer.o: ../src/sumo_timer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sumo_adc.o: ../src/sumo_adc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sumo_irsensor.o: ../src/sumo_irsensor.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sumo_line.o: ../src/sumo_line.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sumo_motor_fs.o: ../src/sumo_motor_fs.c
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
	-rm -rf $(OBJECTS) TDT.elf dep/* TDT.hex TDT.eep

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

