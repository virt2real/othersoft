/*
 *  Copyright (c) 1999-2000 Vojtech Pavlik
 *  Copyright (c) 2009-2011 Red Hat, Inc
 *  modified by Gol
 */

#define _GNU_SOURCE /* for asprintf */
#include <stdio.h>
#include <stdint.h>
#include <linux/version.h>
#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

#define LOWBYTE(v)   ((unsigned char) (v))
#define HIGHBYTE(v)  ((unsigned char) (((unsigned int) (v)) >> 8))

#ifndef EV_SYN
#define EV_SYN 0
#endif
#ifndef SYN_MT_REPORT
#define SYN_MT_REPORT 2
#endif

enum evtest_mode {
    MODE_CAPTURE,
    MODE_QUERY,
    MODE_VERSION,
};

/* verbose block */

int verbose = 0;

/* Joystick state */
unsigned int axis0 = 0, axis1 = 0, axis2 = 0, axis3 = 0, axis4 = 0, axis5 = 0, axis6 = 0, axis7 = 0;
unsigned int buttons = 0;
unsigned char buttonsLow = 0, buttonsHigh = 0;

char updateFlag = 0;

/* Network variables */

char * hostaddr = "localhost";
int sockfd = 0, portno = 0, n;
struct sockaddr_in serv_addr;
struct hostent *server;

char buffer[18];

/* End network variables */

void UpdateJoystickData(void) {

    if (verbose) printf( "X: %d  Y: %d  Rx:%d  Ry:%d  Z: %d  R: %d  PadX: %d PadY: %d buttons: %u\n", axis0, axis1, axis2, axis3, axis4, axis5, axis6, axis7, buttons);

    /* if connected to server - send joy data */
    if (sockfd) {

	sprintf(buffer, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", LOWBYTE(axis0), HIGHBYTE(axis0), LOWBYTE(axis1), HIGHBYTE(axis1), LOWBYTE(axis2), HIGHBYTE(axis2), LOWBYTE(axis3), HIGHBYTE(axis3), LOWBYTE(axis4), HIGHBYTE(axis4), LOWBYTE(axis5), HIGHBYTE(axis5),  LOWBYTE(axis6), HIGHBYTE(axis6), LOWBYTE(axis7), HIGHBYTE(axis7), LOWBYTE(buttons), HIGHBYTE(buttons) );
	n = write(sockfd, buffer, 16);

	if (n < 0)
	    error("ERROR writing to socket");

	bzero(buffer, 18);
    }

}

static int usage(){

    printf ("Usage: -d /dev/input/eventX -h <host> -p <port> -v \n");
    printf ("-d <devicename>\tJoystick event device name\n");
    printf ("-h <host>\tCommand server host\n");
    printf ("-p <port>\tCommand server port\n");
    printf ("-v\t\tVerbose joystick output\n");

    exit(1);

}


static int print_device_info(int fd) {

    int i, j;
    int version;
    unsigned short id[4];
    char name[256] = "Unknown";
    unsigned long bit[EV_MAX][NBITS(KEY_MAX)];

    //ioctl(fd, EVIOCGID, id);
    //printf("Input device ID: bus 0x%x vendor 0x%x product 0x%x version 0x%x\n", id[ID_BUS], id[ID_VENDOR], id[ID_PRODUCT], id[ID_VERSION]);
    ioctl(fd, EVIOCGNAME(sizeof(name)), name);
    printf("\nInput device name: \"%s\"\n", name);
    memset(bit, 0, sizeof(bit));
    ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);

    return 0;
}


static int get_events(int fd) {

    struct input_event ev[64];
    int i, rd;
    unsigned int keynum;

    while (1) {

	rd = read(fd, ev, sizeof(struct input_event) * 64);

	if (rd < (int) sizeof(struct input_event)) continue;

	for (i = 0; i < rd / sizeof(struct input_event); i++) {

	    if (ev[i].type == EV_SYN) {

		/* nothing to do*/

	    } else {

		switch (ev[i].type) {

		    case 3:
			/* axis move */

			switch (ev[i].code) {
				case 0:
					/* X-axe */
					axis0 = ev[i].value;
					updateFlag = 1;
					break;
				case 1:
					/* Y-axe */
					axis1 = ev[i].value;
					updateFlag = 1;
					break;
				case 2:
					/* R-axe */
					axis5 = ev[i].value;
					updateFlag = 1;
					break;
				case 3:
					/* RX-axe */
					axis2 = ev[i].value;
					updateFlag = 1;
					break;
				case 4:
					/* RY-axe */
					axis3 = ev[i].value;
					updateFlag = 1;
					break;
				case 5:
					/* Z-axe */
					axis4 = ev[i].value;
					updateFlag = 1;
					break;
				case 16:
					/* Pad-x */
					axis6 = ev[i].value;
					updateFlag = 1;
					break;
				case 17:
					/* Pad-y */
					axis7 = ev[i].value;
					updateFlag = 1;
					break;

			}

			break;

		    case 1:
			/* buttons pressed */

			keynum = ev[i].code - 304;

			if (ev[i].value)
				buttons |= 1 << keynum;
			else
				buttons &= ~( 1 << keynum );

			updateFlag = 1;

			break;

		}

		if (updateFlag) {

			UpdateJoystickData();
			updateFlag = 0;

		}

		if (verbose) printf("type %d , code %d, value %d\n", ev[i].type, ev[i].code, ev[i].value);

	    }
	}

    }
}

static int test_grab(int fd) {

    int rc;
    rc = ioctl(fd, EVIOCGRAB, (void*)1);
    if (!rc)
    	ioctl(fd, EVIOCGRAB, (void*)0);
    return rc;
}

static int do_capture(const char *device) {

    int fd;
    char *filename;

    filename = strdup(device);

    if (!filename)
	return EXIT_FAILURE;

    if ((fd = open(filename, O_RDONLY)) < 0) {

	perror("evtest");

	if (errno == EACCES && getuid() != 0) 
		fprintf(stderr, "You do not have access to %s. Try running as root instead.\n", filename);
		return EXIT_FAILURE;
    }

    free(filename);

    if (!isatty(fileno(stdout)))
	setbuf(stdout, NULL);

    if (print_device_info(fd))
	return EXIT_FAILURE;

    printf("\nStart reading device...\n");

    if (test_grab(fd)) {
	printf("This device is grabbed by another process.\n");
    }

    return get_events(fd);
}

int main (int argc, char **argv) {

    const char *device = NULL;
    enum evtest_mode mode = MODE_CAPTURE;
    int c;

    if (argc < 2) {
	usage();
    }

	// parse command line params

    while (1) {

	int option_index = 0;
	static struct option long_options[] = { {0, 0, 0, 0} };
	c = getopt_long (argc, argv, "d:h:p:v", long_options, &option_index);
	if (c == -1)
	    break;
	switch (c) {

	    case 'd':
	        device = optarg;
	        break;

	    case 'h':
	        hostaddr = optarg;
	        break;

	    case 'p':
	        portno = atoi(optarg);
	        break;

	    case 'v':
	        verbose = 1;
	        break;

	    default:
	        break;
	}
    }

    if (!device) usage();

    printf ("Using device %s\n", device);
    printf ("Try to command server  %s:%d ", hostaddr, portno);

    /* Open server connection */

    if (portno) {

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
    	    error("ERROR opening socket");
        }

	server = gethostbyname(hostaddr);
        if (server == NULL) {
	    fprintf(stderr,"ERROR, no such host\n");
    	    exit(0);
        }

	bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
    	     (char *)&serv_addr.sin_addr.s_addr,
             server->h_length);
	serv_addr.sin_port = htons(portno);
        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
	    error("ERROR connecting");
	    sockfd = 0;
	    exit(1);
        } else
		printf(" success!\n");

    }

    /* End server connection */

    return do_capture(device);

}


void error(const char *msg) {

    perror(msg);

}
