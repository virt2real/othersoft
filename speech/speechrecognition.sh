#!/bin/sh

# filename
RECFILE=record

echo 0 > /proc/v2r_gpio/pwctr3

while [ 1 ] ; do

	#rec -c 1 -r 16000 -b 16 /tmp/$RECFILE.wav --no-show-progress silence 1 0.1 6% 1 1.0 10%
	#flac -f -s /tmp/$RECFILE.wav -o /tmp/$RECFILE.flac

	./record.php  $RECFILE

	./proceed.php $RECFILE

	if [ $? == 1 ] ; then
		echo "the end"
		exit
	fi

	rm -f /tmp/$RECFILE.flac 
	rm -f /tmp/$RECFILE.wav

done

