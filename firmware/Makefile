# ===============================
# Project configuration
# ===============================
PROJECT = smart_collar
MCU     = atmega328p
F_CPU   = 16000000UL

# ===============================
# Toolchain
# ===============================
CC      = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE    = avr-size
AVRDUDE = avrdude

# ===============================
# Programmer (USBasp)
# ===============================
PROGRAMMER = usbasp
PORT       = usb
BAUD       = 9600

# ===============================
# Files
# ===============================
SRC = src/main.c
OBJ = $(SRC:.c=.o)

# ===============================
# Flags
# ===============================
CFLAGS  = -std=gnu11
CFLAGS += -Wall -Wextra -Wshadow -Wundef
CFLAGS += -Os
CFLAGS += -mmcu=$(MCU)
CFLAGS += -DF_CPU=$(F_CPU)
CFLAGS += -ffunction-sections -fdata-sections

LDFLAGS = -mmcu=$(MCU)
LDFLAGS += -Wl,--gc-sections

# ===============================
# Targets
# ===============================
all: $(PROJECT).hex size

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(PROJECT).elf: $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

$(PROJECT).hex: $(PROJECT).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

size: $(PROJECT).elf
	$(SIZE) --format=avr --mcu=$(MCU) $<

flash: $(PROJECT).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p m328pb -U flash:w:$<:i

clean:
	rm -f $(OBJ) $(PROJECT).elf $(PROJECT).hex
