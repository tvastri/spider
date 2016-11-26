CC = gcc
RM = rm
CFLAGS = -g -Wall -Werror -I/usr/include/glib-2.0/ -I/usr/lib64/glib-2.0/include
LDFLAGS = -lcurl -lssl -lcrypto -lglib-2.0

SOURCES = config.c           \
          debug.c            \
          file_utils.c       \
          server.c           \
          scan.c             \
          main.c

OBJECTS = $(SOURCES:.c=.o)
DEPS = $(SOURCES:.c=.d)

spider-client: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o spider-client 

%.o:%.c %.d

%.d: %.c
	$(CC) $(CFLAGS) -MM $< -o $@

clean:
	@$(RM) -f $(OBJECTS) $(DEPS) spider-client

-include $(DEPS)

