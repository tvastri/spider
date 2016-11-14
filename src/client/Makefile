CFLAGS = -g

spider-client: main.o scan_dir.o debug.o
	gcc main.o scan_dir.o debug.o -o spider-client

clean:
	@rm *.o spider-client
