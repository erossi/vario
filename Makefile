# Copyright (C) 2016 Enrico Rossi
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

PRG_NAME = vario
# space opt
OPTLEV = s

# Platforms:
# - arduino (default)
# - attiny85 (3.3V @8Mhz)
# - attiny85v (1.8V @1Mhz)
ifndef PLATFORM
PLATFORM = arduino
endif

## Microcontroller definition
#
# use make PLATFORM=arduino to compile for arduino
#
ifeq ($(PLATFORM), arduino)
	MCU = atmega328p
	FCPU = 16000000UL
else ifeq ($(PLATFORM), attiny85)
	MCU = $(PLATFORM)
	FCPU = 8000000UL
else ifeq ($(PLATFORM), attiny85v)
	MCU = attiny85
	FCPU = 1000000UL
else
$(error	unsupported platform)
endif

PWD = $(shell pwd)
INC = -I/usr/lib/avr/include/

CFLAGS := $(INC) -Wall -Wstrict-prototypes -pedantic -std=c11 -mmcu=$(MCU) -O$(OPTLEV) -D F_CPU=$(FCPU)

ifeq ($(PLATFORM), arduino)
	CFLAGS += -D ARDUINO
endif

LFLAGS = -lm

# git version
GIT_TAG = "$(shell git describe --tags --long --always)"
PRGNAME = $(PRG_NAME)_$(GIT_TAG)_$(MCU)
ELFNAME = $(PRGNAME)_$@

AR = avr-ar
CC = avr-gcc

# Arduino
DUDEAPORT = /dev/ttyACM0
DUDEADEV = arduino

# Stk500v2
DUDESPORT = /dev/ttyUSB0
DUDESDEV = stk500v2

# avrispmkII
DUDEUPORT = usb
DUDEUDEV = avrispmkII

# Use sudo for USB avrispmkII
DUDE = sudo avrdude -p $(MCU) -e -U flash:w:$(PRGNAME).hex
#DUDE = avrdude -p $(MCU) -e -U flash:w:$(PRGNAME).hex

OBJCOPY = avr-objcopy -j .text -j .data -O ihex
OBJDUMP = avr-objdump
SIZE = avr-size --format=avr --mcu=$(MCU) $(PRGNAME).elf

REMOVE = rm -f

ifeq ($(PLATFORM), arduino)
all_obj = i2c.o lps25.o buzz.o usart.o debug.o
else
all_obj = USI_TWI_Master.o lps25.o buzz.o
endif

test_fifo_mean_obj = i2c.o lps25.o usart.o debug.o
test_lps_obj = i2c.o lps25.o usart.o debug.o logit.o

.PHONY: clean indent
.SILENT: help
.SUFFIXES: .c, .o

## Export variables used in sub-make
#
.EXPORT_ALL_VARIABLES: doc

## Default
#
all: $(all_obj)
ifeq ($(PLATFORM), arduino)
	$(CC) $(CFLAGS) -o $(PRGNAME).elf main_arduino.c $(all_obj) $(LFLAGS)
else
	$(CC) $(CFLAGS) -o $(PRGNAME).elf main_tiny.c $(all_obj) $(LFLAGS)
endif

	$(OBJCOPY) $(PRGNAME).elf $(PRGNAME).hex

test_usi: USI_TWI_Master.o
	$(CC) $(CFLAGS) -o $(PRGNAME).elf test_usi.c USI_TWI_Master.o $(LFLAGS)
	$(OBJCOPY) $(PRGNAME).elf $(PRGNAME).hex

test_lps: $(test_lps_obj)
	$(CC) $(CFLAGS) -o $(PRGNAME).elf test_lps25.c $(test_lps_obj) $(LFLAGS)
	$(OBJCOPY) $(PRGNAME).elf $(PRGNAME).hex

test_fifo_mean: $(test_fifo_mean_obj)
	$(CC) $(CFLAGS) -o $(PRGNAME).elf test_fifo_mean.c $(test_fifo_mean_obj) $(LFLAGS)
	$(OBJCOPY) $(PRGNAME).elf $(PRGNAME).hex

programstk:
	$(DUDE) -c $(DUDESDEV) -P $(DUDESPORT)

program:
ifeq ($(PLATFORM), arduino)
	$(DUDE) -c $(DUDEADEV) -P $(DUDEAPORT)
else
	$(DUDE) -c $(DUDEUDEV) -P $(DUDEUPORT)
endif

## CleanUp and tools
#
clean:
	$(REMOVE) *.elf *.hex *.o $(objects)

version:
	# Last Git tag: $(GIT_TAG)

doc_timing:
	$(MAKE) -C ../doc timing

doc:
	$(MAKE) -C ../doc doc

size:
	$(SIZE)
