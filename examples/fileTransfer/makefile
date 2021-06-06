CFLAGS = -g -Wall

all: client1 client2

client1.o: client1.c
	gcc -c  $(CFLAGS) client1.c

client1: client1.o KVS-lib.a
	gcc client1.o KVS-lib.a -o client1 -lpthread

client2.o: client2.c
	gcc -c  $(CFLAGS) client2.c

client2: client2.o KVS-lib.a
	gcc client2.o KVS-lib.a -o client2 -lpthread
	
clean:
	rm client1 client2 *.o *.a