CROSSCOMPILE?=/opt/SDK/codesourcery/arm-2013.05/bin/arm-none-linux-gnueabi-
CC= $(CROSSCOMPILE)gcc
OBJCOPY= $(CROSSCOMPILE)objcopy
OBJDUMP= $(CROSSCOMPILE)objdump
CFLAGS=-I/opt/SDK/kernel -L/opt/SDK/kernel

all:
#	$(CC) $(CFLAGS) -O2 -o i2c_read ./smbus.c  i2c_read.c
	$(CC) $(CFLAGS) -O2 -o i2c_read i2c_read.c

clean:
	rm -f i2c_read
