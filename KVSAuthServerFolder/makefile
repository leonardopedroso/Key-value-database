CFLAGS = -g -Wall -pedantic

KVSAuthServer-data.o: KVSAuthServer-data.c KVSAuthServer-data.h
	gcc $(CFLAGS) -c KVSAuthServer-data.c

KVSAuthServer-com.o: KVSAuthServer-com.c KVSAuthServer-com.h
	gcc $(CFLAGS) -c KVSAuthServer-com.c

KVSAuthServer.o: KVSAuthServer.c KVSAuthServer-com.o KVSAuthServer-data.o
	gcc $(CFLAGS) -c KVSAuthServer.c

KVSAuthServer: KVSAuthServer.o
	gcc $(CFLAGS) KVSAuthServer.o KVSAuthServer-data.o KVSAuthServer-com.o -o KVSAuthServer

all: KVSAuthServer