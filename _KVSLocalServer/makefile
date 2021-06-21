CFLAGS = -g -Wall

all: KVSLocalServer
	
# ---------- LOCAL SERVER ----------
KVSLocalServer-data.o: KVSLocalServer-data.c KVSLocalServer-data.h KVSLocalServer-auth.h KVSLocalServer-base.h KVS-lib-MACROS.h
	gcc -c $(CFLAGS) KVSLocalServer-data.c

KVSLocalServer-client.o: KVSLocalServer-client.c KVSLocalServer-client.h KVSLocalServer-data.h KVSLocalServer-com.h KVSLocalServer-cb.h KVSLocalServer-base.h KVS-lib-MACROS.h
	gcc -c $(CFLAGS) KVSLocalServer-client.c

KVSLocalServer-com.o: KVSLocalServer-com.c KVSLocalServer-com.h KVSLocalServer-base.h KVS-lib-MACROS.h KVS_local-lib_com.h
	gcc -c $(CFLAGS) KVSLocalServer-com.c

KVSLocalServer-auth.o: KVSLocalServer-auth.c KVSLocalServer-auth.h KVSLocalServer-base.h KVS-lib-MACROS.h KVS_local-auth_com.h
	gcc -c $(CFLAGS) KVSLocalServer-auth.c

KVSLocalServer-cb.o: KVSLocalServer-cb.c KVSLocalServer-cb.h KVSLocalServer-data.h KVSLocalServer-base.h KVS-lib-MACROS.h 
	gcc -c $(CFLAGS)  KVSLocalServer-cb.c

ui.o: ui.c ui.h
	gcc -c $(CFLAGS) ui.c

KVSLocalServer.o: KVSLocalServer.c KVSLocalServer.h KVSLocalServer-data.h KVSLocalServer-client.h KVSLocalServer-cb.h ui.h KVSLocalServer-base.h KVS-lib-MACROS.h 
	gcc -c $(CFLAGS) KVSLocalServer.c

KVSLocalServer: KVSLocalServer.o KVSLocalServer-client.o KVSLocalServer-com.o KVSLocalServer-data.o KVSLocalServer-auth.o KVSLocalServer-cb.o ui.o
	gcc KVSLocalServer.o KVSLocalServer-com.o KVSLocalServer-client.o KVSLocalServer-data.o KVSLocalServer-auth.o KVSLocalServer-cb.o ui.o -o KVSLocalServer -lpthread

clean:
	rm KVSLocalServer client *.o 