/*
	Check I2C device
	by Gol, 2014
*/

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include "i2c-dev.h"

int file;
int i2c_address;
char i2c_device[255];

void usage();
int checkDevice(int file, int addr, char * name);

int main(int argc, char * argv[]) {

	int c;
	int n;

	/* initial values */
	sprintf(i2c_device, "/dev/i2c-1");

	/* parse cmdline */
	while (1) {
		int option_index = 0;
		static struct option long_options[] = { {0, 0, 0, 0} };
		c = getopt_long (argc, argv, "d:a:h?", long_options, &option_index);

		if (c == -1) 
			break;

		switch (c) {
			case 'd':
				n = atoi(optarg);
				sprintf(i2c_device, "/dev/i2c-%d", n);
			break;
			case 'a':
				i2c_address = strtol(optarg, 0, 16);
			break;
			case 'h':
			case '?':
				usage();
				exit(1);
			break;
			default:
			break;
		}
	}

	if (!i2c_address) {
		usage();
		exit(1);
	}

	if ((file = open(i2c_device, O_RDWR)) < 0) {
		printf("Failed to open the I2C device\n");
		exit(2);
	}

	/* check i2c device presence */
	if (!checkDevice(file, i2c_address, "device")) {
		printf("Device 0x%x not found\n", i2c_address);
		exit(1);
	} else {
		printf("Device 0x%x found\n", i2c_address);
		exit(0);
	}

}

int checkDevice(int file, int addr, char * name) {
	if (ioctl(file, I2C_SLAVE, addr) < 0) {
		if (errno == EBUSY) {
			return 1;
		} else {
			return 0;
		}
	} else {
		int res = i2c_smbus_write_quick(file, I2C_SMBUS_WRITE);
		if (res < 0) {
			return 0;
		}
		else
			return 1;
	}
}

/* usage info */
void usage() {
	printf ("I2C device check. Returns 0 if device exists, 1 - if device absent\n");
	printf ("by Gol, 2014\n");
	printf ("Usage: [-d <i2cdevnum>] -a <i2caddr> [h] [?]\n");
	printf ("-d\t\tI2C device number, for example, -d 1 = use device /dev/i2c-1\n");
	printf ("-a\t\tdevice I2C address, for example, -a 0x3c\n");
	printf ("-? or -h\tshow this info\n");
}
