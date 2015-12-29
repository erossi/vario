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
OPTLEV = s

## Microcontroller definition
#
MCU = attiny25
FCPU = 1000000UL

PWD = $(shell pwd)
INC = -I/usr/lib/avr/include/

CFLAGS = $(INC) -Wall -Wstrict-prototypes -pedantic -mmcu=$(MCU) -O$(OPTLEV) -D F_CPU=$(FCPU)
LFLAGS = -lm

# Uncomment if git is in use
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

all_obj =
test_all_obj =

# CFLAGS += -D USE_USART1

.PHONY: clean indent
.SILENT: help
.SUFFIXES: .c, .o

## Export variables used in sub-make
#
.EXPORT_ALL_VARIABLES: doc

## Default
#
all: $(all_obj)
	$(CC) $(CFLAGS) -o $(PRGNAME).elf main.c $(all_obj) $(LFLAGS)
	$(OBJCOPY) $(PRGNAME).elf $(PRGNAME).hex

## Programming part
#
arduino:
	$(DUDE) -c $(DUDEADEV) -P $(DUDEAPORT)

programstk:
	$(DUDE) -c $(DUDESDEV) -P $(DUDESPORT)

program:
	$(DUDE) -c $(DUDEUDEV) -P $(DUDEUPORT)

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