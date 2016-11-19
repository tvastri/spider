CC = gcc
RM = rm
CFLAGS = -I/usr/include/glib-2.0/ -I/usr/lib64/glib-2.0/include
LDFLAGS = -lcurl -lssl -lcrypto -lglib-2.0
DEPDIR = deps

$(shell mkdir -p $(DEPDIR) >/dev/null)

SOURCES = config_utils.c     \
          debug.c            \
          file_utils.c       \
          net_utils.c        \
          scan_dir.c         \
          main.c

OBJECTS = $(SOURCES:.c=.o)
DEPS = $(SOURCES:.c=.d)

spider-client: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o spider-client 

%.o:%.c %.d

%.d: %.c
	$(CC) $(CFLAGS) -MM $< -o $@

#-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(SOURCES)))
-include $(DEPS)

clean:
	@$(RM) -f $(OBJECTS) $(DEPS) spider-client
