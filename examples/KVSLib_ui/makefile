CFLAGS = -g -Wall

all: client KVSLib_ui

KVSLib_ui.o: KVSLib_ui.c
	gcc -c  $(CFLAGS) KVSLib_ui.c

client.o: client.c
	gcc -c  $(CFLAGS) client.c

cliet: client.o KVSLib_ui.o KVS-lib.a
	gcc client.o KVS-lib.a KVSLib_ui.o -o client
	
clean:
	rm client *.o