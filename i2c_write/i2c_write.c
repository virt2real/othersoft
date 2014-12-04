/*
	I2C registers writer values from text file
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
char * buffer;
FILE *readf;
char filename[255];
int delayvalue;

void usage();
void selectDevice(int file, int addr, char * name);
void writeToDevice(int file, char * buf, int len);
int checkDevice(int file, int addr, char * name);
void parseRegisters(char * buffer);

int main(int argc, char * argv[]) {

	long filesize;
	int c;
	int n;

	/* initial values */
	sprintf(i2c_device, "/dev/i2c-1");
	delayvalue = 10;

	/* parse cmdline */
	while (1) {
		int option_index = 0;
		static struct option long_options[] = { {0, 0, 0, 0} };
		c = getopt_long (argc, argv, "d:a:f:s:h?", long_options, &option_index);

		if (c == -1) 
			break;

		switch (c) {
			case 'd':
				n = atoi(optarg);
				sprintf(i2c_device, "/dev/i2c-%d", n);
			break;
			case 's':
				delayvalue = atoi(optarg);
			break;
			case 'a':
				i2c_address = strtol(optarg, 0, 16);
			break;
			case 'f':
				sprintf(filename, "%s", optarg);
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

	if (!strcmp(filename,"")) {
		usage();
		exit(1);
	}

	/* check registers file exists */
	if(access(filename, F_OK ) == -1 ) {
		printf("File %s not found\n", filename);
		exit(1);
	}

	/* get file size */
	struct stat st;
	stat(filename, &st);
	filesize = st.st_size;
	if (!filesize) {
		printf("Empty registers file\n");
		exit(1);
	}

	/* allocate memory for file buffer */
	buffer = (char*) malloc(sizeof(char)*(filesize));
	if (!buffer) {
		printf("Can't allocate memory\n");
		exit(1);
	}

	/* read registers from file */
	readf=fopen(filename, "r");
	fread(buffer, sizeof(char), filesize, readf);

	if ((file = open(i2c_device, O_RDWR)) < 0) {
		printf("Failed to open the I2C device\n");
		free(buffer);
		exit(1);
	}

	printf("Using I2C bus %s\n", i2c_device);

	/* check i2c device presence */
	if (!checkDevice(file, i2c_address, "device")) {
		printf("Device 0x%x not found\n", i2c_address);
		exit(1);
	} else {
		printf("Device 0x%x found\n", i2c_address);
	}

	printf("Writing registers from file %s to I2C address 0x%x...\n", filename, i2c_address);

	/* select i2c device */
	selectDevice(file, i2c_address, "device");

	/* parse registers file */
	parseRegisters(buffer);

	printf("Done!\n");

	free(buffer);
}


void parseRegisters(char * buffer) {
    char *p;
    char *temp_string;
    char *temp_param;
    char *parts;
	int i;

	unsigned char values[3];
	unsigned char reg_lo_addr;
	unsigned char reg_hi_addr;
	unsigned char reg_value;

	char buf[3];

    temp_string = strdup(buffer);

    do {
		p = strsep(&temp_string, "\n");
		if (p) {
		    /* split strings by spaces */
		    temp_param = strdup(p);
			if (!temp_param) continue;

			memset(values, 0, sizeof(values));
			i = 0;
			do {

		    	parts = strsep(&temp_param, " ");
				if (!parts) break;

				/* skip comments */
				if (parts[0] == '#') {
					break;
				}

				/* make integers from strings (like 0x00) */
				values[i] = strtol(parts, 0, 16);
				i++;

			} while (parts);

			/* check params values and detect one or two byte register address*/
			switch (i) {
				default:
				case 0:
				case 1:
					/* empty or wrong values, skip*/
				break;
				case 2:
					reg_lo_addr = values[0];
					reg_value = values[1];
					buf[0] = reg_lo_addr;
					buf[1] = reg_value;
					writeToDevice(file, buf, 2);
					usleep(10);
				break;
				case 3:
					reg_hi_addr = values[0];
					reg_lo_addr = values[1];
					reg_value = values[2];
					buf[0] = reg_hi_addr;
					buf[1] = reg_lo_addr;
					buf[2] = reg_value;
					writeToDevice(file, buf, 3);
					usleep(delayvalue);
				break;
			}

		}

    } while(p);

}

void writeToDevice(int file, char * buf, int len) {
	if (write(file, buf, len) != len) {
		printf("Can't write to device\n");
	}
}

void selectDevice(int file, int addr, char * name) {
	if (ioctl(file, I2C_SLAVE_FORCE, addr) < 0) {
		printf("\n%s 0x%x not present\n", name, i2c_address);
		exit(1);
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
		union i2c_smbus_data data;
		int res = i2c_smbus_access(file, I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data);
		if (res < 0)
			return 0;
		else
			return 1;
	}
}

/* usage info */
void usage() {
	printf ("I2C registers write\n");
	printf ("by Gol, 2014\n");
	printf ("Usage: [-d <i2cdevnum>] -a <i2caddr> -f <filename> -p <pause> [h] [?]\n");
	printf ("-d\t\tI2C device number, for example, -d 1 = use device /dev/i2c-1\n");
	printf ("-a\t\tdevice I2C address, for example, -a 0x3c\n");
	printf ("-f\t\tfilename with registers values\n");
	printf ("-s\t\tmake pause (at microseconds) after every write, for example, -s 10\n");
	printf ("-? or -h\tshow this info\n");
}
