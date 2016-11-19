#CFLAGS = -g
LDFLAGS = -lcurl -lssl -lcrypto

SOURCES = debug.c file_utils.c net_utils.c scan_dir.c main.c

OBJECTS = $(SOURCES:.c=.o)

spider-client: $(OBJECTS)
	gcc $(OBJECTS) $(LDFLAGS) -o spider-client 

clean:
	@rm -f $(OBJECTS) spider-client
