CFLAGS = -g -Wall

all: KVSAuthServer

KVSAuthServer-data.o: KVSAuthServer-data.c KVSAuthServer-data.h KVS-lib-MACROS.h
	gcc $(CFLAGS) -c KVSAuthServer-data.c

KVSAuthServer-com.o: KVSAuthServer-com.c KVSAuthServer-com.h KVS_local-auth_com.h KVS-lib-MACROS.h
	gcc $(CFLAGS) -c KVSAuthServer-com.c

KVSAuthServer.o: KVSAuthServer.c KVSAuthServer-com.h KVSAuthServer-data.h KVS_local-auth_com.h KVS-lib-MACROS.h
	gcc $(CFLAGS) -c KVSAuthServer.c

KVSAuthServer: KVSAuthServer.o KVSAuthServer-com.o KVSAuthServer-data.o
	gcc $(CFLAGS) KVSAuthServer.o KVSAuthServer-data.o KVSAuthServer-com.o -o KVSAuthServer -lpthread

clean: 
	rm *.o KVSAuthServer