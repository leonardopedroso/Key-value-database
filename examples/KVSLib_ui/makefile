CFLAGS = -g -Wall

all: client

KVSLib_ui.o: KVSLib_ui.c KVS-lib-MACROS.h KVS-lib.h
	gcc -c  $(CFLAGS) KVSLib_ui.c

client.o: client.c KVS-lib-MACROS.h KVS-lib.h
	gcc -c  $(CFLAGS) client.c

client: client.o KVSLib_ui.o KVS-lib.a
	gcc client.o KVSLib_ui.o KVS-lib.a -o client -lpthread
	
clean:
	rm client *.o