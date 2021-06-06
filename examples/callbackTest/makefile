CFLAGS = -g -Wall

all: client clientCreateKey

client.o: client.c
	gcc -c  $(CFLAGS) client.c

clientCreateKey.o: clientCreateKey.c
	gcc -c $(CFLAGS) clientCreateKey.c

clientCreateKey:  clientCreateKey.o
	gcc clientCreateKey.o KVS-lib.a -o clientCreateKey -lpthread

client: client.o KVS-lib.a
	gcc client.o KVS-lib.a -o client -lpthread
	
clean:
	rm client *.o clientCreateKey