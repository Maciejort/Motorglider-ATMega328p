NAME = main
SERIAL = COM3
ARDUINO = -F -V -c arduino -P $(SERIAL) -b 115200
USBASP = avrdude -c usbasp
AVRDUDE = avrdude $(ARDUINO)

DEF = -DF_CPU=16000000UL -D__AVR_ATmega328P__

SRC = \
main.c

INC = \
-I.

.PHONY : build flash flase run erase

.DEFAULT_GOAL := run

run :
	@$(MAKE) build
	@$(MAKE) flash

build : $(NAME).elf
	avr-objcopy -j .text -j .data -O ihex $< $(NAME).hex

flash :
	$(AVRDUDE) -p ATMEGA328P -U flash:w:$(NAME).hex:i

flase : flash

erase :
	$(AVRDUDE) -p ATMEGA328P -e

$(NAME).elf : $(NAME).o
	avr-gcc -Os $(DEF) -mmcu=atmega328p $(SRC) $(INC) -o $@

$(NAME).o : $(SRC)
	avr-gcc -g -Os $(DEF) -mmcu=atmega328p $^ $(INC) -o $@

include assets/fetch.mk