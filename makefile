all: client KVSLocalServer

KVS-lib.o: KVS-lib.c KVS-lib.h
		gcc -c KVS-lib.c

client.o: client.c
	gcc -c client.c

client: client.o KVS-lib.o  
	gcc client.o KVS-lib.o -o client

KVSLocalServer: KVSLocalServer.c KVSLocalServer.h
	gcc KVSLocalServer.c -o KVSLocalServer
