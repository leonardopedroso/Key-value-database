CFLAGS = -g -Wall -pedantic

all: client

client.o: client.c KVS_local-auth_com.h
	gcc $(CFLAGS) -c client.c

KVS_LocalServer-auth.o: KVS_local-auth_com.h KVSLocalServer-auth.c KVSLocalServer-auth.h
	gcc $(CFLAGS) -c KVSLocalServer-auth.c

client: client.o KVS_LocalServer-auth.o
	gcc $(CFLAGS) client.o KVSLocalServer-auth.o -o client -lpthread -lrt