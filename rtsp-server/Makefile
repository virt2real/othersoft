LDFLAGS=-lgstrtspserver-0.10 -lpcre -lgstbase-0.10 -lgstrtp-0.10 -lgstrtsp-0.10 -lgstsdp-0.10 -lgstapp-0.10 -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lglib-2.0 -lffi
all:
	$(CC) $(CFLAGS) -O2 -o rtsp-server rtsp-server.c $(LDFLAGS)

clean:
	rm -f rtsp-server


#CFLAGS=-I$(ROOT)/fs/output/staging/usr/include -I$(ROOT)/fs/output/staging/usr/include/gstreamer-0.10 -I$(ROOT)/fs/output/staging/usr/include/glib-2.0 -I$(ROOT)/fs/output/host/usr/lib/glib-2.0/include -I$(ROOT)/fs/output/build/gst-rtsp-server-0.10.8
#LDFLAGS=-lgstrtspserver-0.10 -lpcre -lgstbase-0.10 -lgstrtp-0.10 -lgstrtsp-0.10 -lgstsdp-0.10 -lgstapp-0.10 -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lglib-2.0 -lffi -L$(ROOT)/fs/output/target/usr/lib
