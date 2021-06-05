CFLAGS = -g -Wall -pedantic

KVSAuthServer-data.o: KVSAuthServer-data.c KVSAuthServer-data.h
	gcc $(CFLAGS) -c KVSAuthServer-data.c

KVSAuthServer.o: KVSAuthServer.c KVSAuthServer.h KVSAuthServer-data.h KVSAuthServer-data.o
	gcc $(CFLAGS) -c KVSAuthServer.c

KVSAuthServer: KVSAuthServer.o
	gcc $(CFLAGS) KVSAuthServer.o KVSAuthServer-data.o -o KVSAuthServer -lpthread