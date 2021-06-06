CFLAGS = -g -Wall

all: client

client.o: client.c
	gcc -c  $(CFLAGS) client.c

client: client.o KVS-lib.a
	gcc client.o KVS-lib.a -o client -lpthread
	
clean:
	rm client *.o 