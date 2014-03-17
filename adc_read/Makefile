CROSSCOMPILE?=/opt/SDK6/codesourcery/arm-2013.05/bin/arm-none-linux-gnueabi-
CC= $(CROSSCOMPILE)gcc
OBJCOPY= $(CROSSCOMPILE)objcopy
OBJDUMP= $(CROSSCOMPILE)objdump
CFLAGS=-I/opt/SDK6/kernel -L/opt/SDK6/kernel

all:
	$(CC) $(CFLAGS) -O2 -o adc_read adc_read.c

clean:
	rm -f adc_read
