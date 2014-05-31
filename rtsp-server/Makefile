LDFLAGS=-lgstrtspserver-0.10 -lpcre -lgstbase-0.10 -lgstrtp-0.10 -lgstrtsp-0.10 -lgstsdp-0.10 -lgstapp-0.10 -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lglib-2.0 -lffi -L$(TARGET_DIR)/usr/lib
all:
	$(CC) $(CFLAGS) -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/gstreamer-0.10 -I$(STAGING_DIR)/usr/include/glib-2.0 -I$(HOST_DIR)/usr/lib/glib-2.0/include -I$(BASE_DIR)/build/gst-rtsp-server-0.10.8 -O2 -o rtsp-server rtsp-server.c $(LDFLAGS)

clean:
	rm -f rtsp-server
