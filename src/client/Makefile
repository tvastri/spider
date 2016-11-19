CFLAGS = -g

spider-client: main.o scan_dir.o debug.o net_utils.o file_utils.o
	gcc main.o scan_dir.o debug.o net_utils.o file_utils.o -lcurl -o spider-client 

clean:
	@rm *.o spider-client
