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

#define I2CDEV "/dev/i2c-1"

#define ADXL345_I2C_ADDR  0x53
#define HMC5883L_I2C_ADDR 0x1E
#define ITG3200_I2C_ADDR  0x68
#define BMA180_I2C_ADDR  0x40

int file;

int main(void) {

  int data;
  short x, y, z;
  float xa, ya, za;
  unsigned char buf[16];
  int count, b;

  if ((file = open(I2CDEV, O_RDWR)) < 0) {
    printf("Failed to open the bus.");
    exit(1);
  }

  /* initialise ADXL345 */
  selectDevice(file, ADXL345_I2C_ADDR, "ADXL345");
  writeToDevice(file, "\x2d\x00", 2);
  writeToDevice(file, "\x2d\x10", 2);
  writeToDevice(file, "\x2d\x08", 2);
  writeToDevice(file, "\x31\x00", 2);
  writeToDevice(file, "\x31\x0b", 2);

  /* initialise HMC5883L */
  selectDevice(file, HMC5883L_I2C_ADDR, "HMC5883L");
  writeToDevice(file, "\x02\x00", 2);

  /* initialise ITG3200 */
  selectDevice(file, ITG3200_I2C_ADDR, "ITG3200");
  //writeToDevice(file, "\x16\b00011000", 2);
  writeToDevice(file, "\x3E\x00", 2);
  writeToDevice(file, "\x15\x07", 2);
  writeToDevice(file, "\x16\x1E", 2);
  writeToDevice(file, "\x17\x00", 2);

  /* initialise BMA180 */
  selectDevice(file, BMA180_I2C_ADDR, "BMA180");
  writeToDevice(file, "\x10\xB6", 2); // wake up mode
  writeToDevice(file, "\x0D\x10", 2); // low pass filter

  while (1) {

      selectDevice(file, HMC5883L_I2C_ADDR, "HMC5883L");
      writeToDevice(file, "\x03", 1);
   
      if (read(file, buf, 6) != 6) {
         printf("Unable to read from HMC5883L\n");
      }
      else {
         x = buf[1]<<8| buf[0];
         y = buf[3]<<8| buf[2];
         z = buf[5]<<8| buf[4];
         xa = (90.0 / 256.0) * (float) x;
         ya = (90.0 / 256.0) * (float) y;
         za = (90.0 / 256.0) * (float) z;
         //printf("HMC: x=%d, y=%d, z=%d  xa=%4.0f ya=%4.0f za=%4.0f\n", x, y, z, xa, ya, za);
         printf("HMC: x=%d, y=%d, z=%d\n", x, y, z);
      }


      selectDevice(file, BMA180_I2C_ADDR, "BMA180");
      writeToDevice(file, "\x02", 1);
   
      if (read(file, buf, 6) != 6) {
         printf("Unable to read from BMA180\n");
      }
      else {
         x = buf[1]<<8| buf[0];
         y = buf[3]<<8| buf[2];
         z = buf[5]<<8| buf[4];
         printf("BMA180: x=%d, y=%d, z=%d\n", x, y, z);
      }



/*
      selectDevice(file, ADXL345_I2C_ADDR, "ADXL345");
      writeToDevice(file, "\x32", 1);
   
      if (read(file, buf, 6) != 6) {
         printf("Unable to read from ADXL345\n");
      } 
      else {
         x = buf[1]<<8| buf[0];
         y = buf[3]<<8| buf[2];
         z = buf[5]<<8| buf[4];
         xa = (90.0 / 256.0) * (float) x;
         ya = (90.0 / 256.0) * (float) y;
         za = (90.0 / 256.0) * (float) z;
         //printf("ADXL %d %d %d, %4.0f %4.0f %4.0f\n", x, y, z, xa, ya, za);
         printf("ADXL %d %d %d\n", x, y, z);
      }

*/
      selectDevice(file, ITG3200_I2C_ADDR, "ITG3200");
      writeToDevice(file, "\x1D", 1);
   
      if (read(file, buf, 6) != 6) {
         printf("Unable to read from ITG3200\n");
      }
      else {
         x = buf[0]<<8| buf[1];
         y = buf[2]<<8| buf[3];
         z = buf[4]<<8| buf[5];
         xa = (90.0 / 256.0) * (float) x;
         ya = (90.0 / 256.0) * (float) y;
         za = (90.0 / 256.0) * (float) z;
         //printf("ITG: x=%d, y=%d, z=%d xa=%4.0f ya=%4.0f za=%4.0f\n", x, y, z, xa, ya, za);
         printf("ITG: x=%d, y=%d, z=%d\n", x, y, z);
      }

      usleep(10000);
   }

}

void writeToDevice(int file, char * buf, int len) {
   if (write(file, buf, len) != len) {
      printf("Can't write to device\n");
   }
}

void selectDevice(int file, int addr, char * name) {
   if (ioctl(file, I2C_SLAVE, addr) < 0) {
      printf("%s not present\n", name);
   }
}
