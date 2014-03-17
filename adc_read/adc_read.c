#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ADC_CHANNELS	6

int file;
double vref = 3.6;

int main(void) {

	unsigned char buf[ADC_CHANNELS * 2];
	int value;
	char i;

	if ((file = open("/dev/v2r_adc", O_RDWR)) < 0) {
    	printf("Failed to open ADC driver\n");
		exit(1);
	}

	while (1) {

		if (read(file, buf, 12) == 12) {

			for (i = 0; i < ADC_CHANNELS; i++){
				value = buf[i * 2] | (buf[i * 2 + 1] << 8);

				double max_raw = 1 << 10;
				double volts = (double) value * vref / max_raw;

				printf("%d (%1.3fV)  ", value, volts);
			}

			lseek(file, 0, SEEK_SET); 
			usleep(10000);
			printf("\n");
		} 
	}
}
