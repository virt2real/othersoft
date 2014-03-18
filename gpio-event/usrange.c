#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

#include "gpio-event-drv.h"

#define INT_NUM	8

long prev_usec[INT_NUM+1];
long prev_sec[INT_NUM+1];


int main( int argc, char **argv )
{
    FILE               *fs;

    // check device driver file
    if (( fs = fopen( "/dev/gpio-event", "r" )) == NULL )
    {
        perror( "Check to make sure gpio_event_drv has been loaded. Unable to open /dev/gpio-event" );
        exit( 1 );
    }

    // set binary read mode
    ioctl( fileno( fs ), GPIO_EVENT_IOCTL_SET_READ_MODE, 1 );

    printf("reading events:\n");

    while ( 1 )
    {
        ssize_t numBytes;
        char    argStr[ 60 ];

        argStr[ 0 ] = '\0';

        GPIO_Event_t    gpioEvent;

        if (( numBytes = fread( &gpioEvent, 1, sizeof( gpioEvent ), fs )) == sizeof( gpioEvent ))
        {
            snprintf( argStr, sizeof( argStr ), "%2d %c %ld.%06ld",
				gpioEvent.gpio,
				(( gpioEvent.edgeType == GPIO_EventRisingEdge ) ? 'R' : 'F' ),
				gpioEvent.time.tv_sec,
				gpioEvent.time.tv_usec
			);

	    	//printf("%s\n", argStr);

	    	// save previous usec
	    	if (gpioEvent.edgeType == GPIO_EventRisingEdge) {
				prev_sec[gpioEvent.gpio] = gpioEvent.time.tv_sec;
				prev_usec[gpioEvent.gpio] = gpioEvent.time.tv_usec;
	    	} else {
				if (prev_sec[gpioEvent.gpio] > 0 && prev_usec[gpioEvent.gpio] > 0) {
		    		int delta = ( ( gpioEvent.time.tv_sec - prev_sec[gpioEvent.gpio] ) * 1000000 + gpioEvent.time.tv_usec - prev_usec[gpioEvent.gpio]) / 58;

		    		if (delta < 1000) 
		    		{
						printf( "gpio %d: distance: %d\n", gpioEvent.gpio, delta );
		    		}
		    		prev_sec[gpioEvent.gpio] = 0;
		    		prev_usec[gpioEvent.gpio] = 0;
				}
	    	}
		}
    }

    fclose( fs );

    exit( 0 );
    return 0;

}
