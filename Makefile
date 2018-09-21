### vvv EDIT BELOW vvv ###
MCU_TARGET      = attiny13
CLK_FREQ        = 9600000
LOW_FUSE        = 0x7a
HIGH_FUSE       = 0xff

PROG_DEVICE     = usbasp
### ^^^ EDIT ABOVE ^^^ ###

### AVRDUDE OPTIONS
DUDE_OPTIONS = -p $(MCU_TARGET) -c $(PROG_DEVICE) -B 10

PRG            = main
OBJ            = main.o

### GCC OPTIONS
CC             = avr-gcc
OPTIMIZE       = -O2
DEFS           = -DF_CPU=$(CLK_FREQ)
LIBS           =
OBJCOPY        = avr-objcopy

# Override is only needed by avr-lib build system.
override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) -std=c99
override LDFLAGS       = -Wl,-Map,$(PRG).map


all: $(PRG).elf

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@


.PHONY: clean
clean:
	rm -rf *.o $(PRG).elf *.bak *.hex *.map 

.PHONY: upload
upload: $(PRG).hex
	avrdude $(DUDE_OPTIONS) -U flash:w:$(PRG).hex
	
.PHONY: fuse
fuse:
	avrdude $(DUDE_OPTIONS) -U lfuse:w:$(LOW_FUSE):m -U hfuse:w:$(HIGH_FUSE):m

