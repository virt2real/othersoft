/*
	HDMI EDID registers values writer from text file
	by Gol, 2015
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

#define io_i2caddr 0x4c
#define dpll_i2caddr 0x3F
#define cec_i2caddr  0x40
#define info_i2caddr 0x3E
#define ksv_i2caddr 0x32
#define edid_i2caddr 0x36
#define hdmi_i2caddr 0x34
#define cp_i2caddr 0x22


int file;
char i2c_device[255];
char * buffer;
FILE *readf;
char filename[255];
int verbose = 0;

int i;
int counter;
unsigned char edid_data[1024];

void usage();
void selectDevice(int file, int addr, char * name);
void writeToDevice(int file, char * buf, int len);
int checkDevice(int file, int addr, char * name);
void parseEDID(char * buffer);

int main(int argc, char * argv[]) {

	long filesize;
	int c;
	int n;

	/* initial values */
	sprintf(i2c_device, "/dev/i2c-1");

	/* parse cmdline */
	while (1) {
		int option_index = 0;
		static struct option long_options[] = { {0, 0, 0, 0} };
		c = getopt_long (argc, argv, "f:d:vh?", long_options, &option_index);

		if (c == -1) 
			break;

		switch (c) {
			case 'd':
				n = atoi(optarg);
				sprintf(i2c_device, "/dev/i2c-%d", n);
			break;
			case 'f':
				sprintf(filename, "%s", optarg);
			break;
			case 'v':
				verbose = 1;
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

	if (verbose) printf("Using I2C bus %s\n", i2c_device);

	/* check i2c device presence */
	if (!checkDevice(file, io_i2caddr, "device")) {
		printf("Device 0x%x not found\n", io_i2caddr);
		exit(1);
	} else {
		if (verbose) printf("Device 0x%x found\n", io_i2caddr);
	}

	if (verbose) printf("Writing registers from file %s to I2C address 0x%x...\n", filename, io_i2caddr);

	/* parse registers file */
	parseEDID(buffer);

	/*for (i = 0; i < counter; i++)
		printf("%d ", edid_data[i]);
	printf("\n");*/

	if (verbose) printf("Reset ADV7611\n");
	hdmi_reset();

	if (verbose) printf("Init ADV7611\n");
	hdmi_init();

	if (verbose) printf("Write EDID\n");
	write_edid(counter);

	if (verbose) printf("Done!\n");

	free(buffer);
}

void hdmi_reset() {
	system("echo 0 > /proc/v2r_gpio/98");
	system("echo 1 > /proc/v2r_gpio/98");
	system("echo 0 > /proc/v2r_gpio/99");
	system("echo 1 > /proc/v2r_gpio/99");
	system("echo 0 > /proc/v2r_gpio/pwctr2");
	usleep(1000000);
	system("echo 1 > /proc/v2r_gpio/pwctr2");
}

void hdmi_init() {
	
	adv7611_i2c_write_reg(io_i2caddr, 0x40, 0x81);
	adv7611_i2c_write_reg(io_i2caddr, 0x01, 0x05); // TV Frameformat
	adv7611_i2c_write_reg(io_i2caddr, 0x00, 0x19); // 720p with 2x1 decimation
	adv7611_i2c_write_reg(io_i2caddr, 0x02, 0xf5); // YUV out
	adv7611_i2c_write_reg(io_i2caddr, 0x03, 0x00);
	adv7611_i2c_write_reg(io_i2caddr, 0x05, 0x2c);
	adv7611_i2c_write_reg(io_i2caddr, 0x06, 0xa6); // Invert HS, VS pins

	/* Bring chip out of powerdown and disable tristate */
	adv7611_i2c_write_reg(io_i2caddr, 0x0b, 0x44);
	adv7611_i2c_write_reg(io_i2caddr, 0x0c, 0x42);
	adv7611_i2c_write_reg(io_i2caddr, 0x14, 0x3f);
	adv7611_i2c_write_reg(io_i2caddr, 0x15, 0xBE);

	/* LLC DLL enable */
	// adv7611_i2c_write_reg(io_i2caddr, 0x19, 0x83);
	adv7611_i2c_write_reg(io_i2caddr, 0x19, 0xC0);
	adv7611_i2c_write_reg(io_i2caddr, 0x33, 0x40);

	adv7611_i2c_write_reg(io_i2caddr, 0xfd, cp_i2caddr << 1);
	adv7611_i2c_write_reg(io_i2caddr, 0xf9, ksv_i2caddr << 1);
	adv7611_i2c_write_reg(io_i2caddr, 0xfb, hdmi_i2caddr << 1);
	adv7611_i2c_write_reg(io_i2caddr, 0xfa, edid_i2caddr << 1);
                
	/* Force HDMI free run */
	adv7611_i2c_write_reg(cp_i2caddr, 0xba, 0x01);

	/* Disable HDCP 1.1*/
	adv7611_i2c_write_reg(ksv_i2caddr, 0x40, 0x81);

	/* ADI recommended writes */
	adv7611_i2c_write_reg(hdmi_i2caddr, 0x9B, 0x03);
	adv7611_i2c_write_reg(hdmi_i2caddr, 0xC1, 0x01);
	adv7611_i2c_write_reg(hdmi_i2caddr, 0xC2, 0x01);
	adv7611_i2c_write_reg(hdmi_i2caddr, 0xC3, 0x01);
	adv7611_i2c_write_reg(hdmi_i2caddr, 0xC4, 0x01);
	adv7611_i2c_write_reg(hdmi_i2caddr, 0xC5, 0x01);
	adv7611_i2c_write_reg(hdmi_i2caddr, 0xC6, 0x01);
	adv7611_i2c_write_reg(hdmi_i2caddr, 0xC7, 0x01);
	adv7611_i2c_write_reg(hdmi_i2caddr, 0xC8, 0x01);
	adv7611_i2c_write_reg(hdmi_i2caddr, 0xC9, 0x01);
	adv7611_i2c_write_reg(hdmi_i2caddr, 0xCA, 0x01);
	adv7611_i2c_write_reg(hdmi_i2caddr, 0xCB, 0x01);
	adv7611_i2c_write_reg(hdmi_i2caddr, 0xCC, 0x01);

	adv7611_i2c_write_reg(hdmi_i2caddr, 0x00, 0x00); // Set HDMI port A
	adv7611_i2c_write_reg(hdmi_i2caddr, 0x83, 0xFE); // Enable clock terminator for port A
	adv7611_i2c_write_reg(hdmi_i2caddr, 0x6F, 0x08); // ADI recommended setting
	adv7611_i2c_write_reg(hdmi_i2caddr, 0x85, 0x1F); // ADI recommended setting
	adv7611_i2c_write_reg(hdmi_i2caddr, 0x87, 0x70); // ADI recommended setting
	adv7611_i2c_write_reg(hdmi_i2caddr, 0x8D, 0x04); // LFG
	adv7611_i2c_write_reg(hdmi_i2caddr, 0x8E, 0x1E); // HFG
	adv7611_i2c_write_reg(hdmi_i2caddr, 0x1A, 0x8A); // unmute audio
	adv7611_i2c_write_reg(hdmi_i2caddr, 0x57, 0xDA); // ADI recommended setting
	adv7611_i2c_write_reg(hdmi_i2caddr, 0x58, 0x01); // ADI recommended setting
	adv7611_i2c_write_reg(hdmi_i2caddr, 0x75, 0x10); // DDC drive strength
}

void write_edid(int count) {
	int err;

	if (verbose) printf("Write edid controller\n");

	/* Disable I2C access to internal EDID ram from DDC port */
	/* Disable HDCP 1.1 features */
	/* Disable the Internal EDID */
	/* for all ports */

	/* select i2c device */
    err = adv7611_i2c_write_reg(ksv_i2caddr, 0x40, 0x81);
    if (err < 0) {
    	printf("failed to disable HDCP 1.1 features\n");
    	return;
    }

	if (verbose) printf("HDCP 1.1 features disabled\n");
    err = adv7611_i2c_write_reg(ksv_i2caddr, 0x74, 0x00);
    if (err < 0) {
    	printf("fail to reset ksv controller\n");
    	return;
    }

	if (verbose) printf("KSV controller is in reset\n");

	if (verbose) printf("Write edid data %d bytes\n", count);

	//Write EDID block
	for (i = 0; i < count; i++){
		//printf("EDID %x, %x\n", i, edid_data[i]);
	    err = adv7611_i2c_write_reg(edid_i2caddr, i, edid_data[i]);
	    if (err < 0) {
	    	printf("fail to write edid data\n");
	    	return;

	    	/* ADV761x calculates the checksums and enables I2C access
	    	 * to internal EDID ram from DDC port.
	    	 */
	    }
	}

	if (verbose) printf("KSV controller is out of reset\n");
    err = adv7611_i2c_write_reg(ksv_i2caddr, 0x74, 0x01);
    if (err < 0) {
    	printf("fail to set ksv controller\n");
    	return;
    }

	err = adv7611_i2c_write_reg(io_i2caddr, 0x15, 0xBE);
}

void parseEDID(char * buffer) {
    char *p;
    char *temp_string;
    char *temp_param;
    char *parts;

	unsigned char values[3];
	unsigned char reg_lo_addr;
	unsigned char reg_hi_addr;
	unsigned char reg_value;

	int current_register;
	unsigned char current_value;

	char buf[3];

	counter = 0;

    temp_string = strdup(buffer);

    do {
		p = strsep(&temp_string, "\n");
		if (p) {
		    /* split strings by "," */
		    temp_param = strdup(p);
			if (!temp_param) continue;

			memset(values, 0, sizeof(values));
			i = 0;
			do {

		    	parts = strsep(&temp_param, ",");
				if (!parts) break;

				/* skip comments */
				if (parts[0] == '#') {
					break;
				}

				if (strlen(parts) < 2) continue; // skip empty parts
				if (strstr (parts, "/") != NULL) continue; // skip comments

				/* make integers from strings (like 0x00) */
				//values[i] = strtol(parts, 0, 16);

				//current_register = startregister + counter;
				current_value = strtol(parts, 0, 16);
				//if (verbose) printf("reg 0x%x: %s - %d\n", current_register, parts, current_value);
				edid_data[counter] = current_value;

				counter++;

			} while (parts);

		}

    } while(p);

}

int adv7611_i2c_write_reg(int i2caddr, int reg, int val) {
	selectDevice(file, i2caddr, "device");
	return i2c_smbus_write_byte_data(file, reg, val);
}

void writeToDevice(int file, char * buf, int len) {
	if (write(file, buf, len) != len) {
		printf("Can't write to device\n");
	}
}

void selectDevice(int file, int addr, char * name) {
	if (ioctl(file, I2C_SLAVE_FORCE, addr) < 0) {
		printf("\n%s 0x%x not present\n", name, addr);
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
	printf ("HDMI EDID registers write. Very ugly, but it works!\n");
	printf ("by Gol, 2015\n");
	printf ("Usage: -f <filename> [-d <i2cdevnum>] [vh?]\n");
	printf ("-d\tI2C device number, for example, -d 1 = use device /dev/i2c-1\n");
	printf ("-f\tfilename with registers values\n");
	printf ("-v\tverbose\n");
	printf ("-h\tshow this info\n");
	printf ("-?\tshow this info\n");
}
