CFLAGS = -I/usr/include/glib-2.0/ -I/usr/lib64/glib-2.0/include

LDFLAGS = -lcurl -lssl -lcrypto -lglib-2.0

SOURCES = config.c     \
          debug.c      \
          file_utils.c \
          net_utils.c  \
          scan_dir.c   \
          main.c

OBJECTS = $(SOURCES:.c=.o)

spider-client: $(OBJECTS)
	gcc $(OBJECTS) $(LDFLAGS) -o spider-client 

clean:
	@rm -f $(OBJECTS) spider-client
