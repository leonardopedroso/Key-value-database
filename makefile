all: client KVSLocalServer

# ---------- KVS-LIB ----------
KVS-lib-com.o: KVS-lib-com.c KVS-lib-com.h KVS-lib-base.h KVS-lib-MACROS.h
	gcc -c -g KVS-lib-com.c
KVS-lib-cb.o: KVS-lib-cb.c KVS-lib-cb.h KVS-lib-base.h KVS-lib-MACROS.h
	gcc -c -g KVS-lib-cb.c
KVS-lib.o: KVS-lib.c KVS-lib.h KVS-lib-com.c KVS-lib-cb.h KVS-lib-base.h KVS-lib-MACROS.h
	gcc -c -g KVS-lib.c

# ---------- CLIENT ----------
client.o: client.c
	gcc -c -g client.c

client: client.o KVS-lib.o KVS-lib-com.o KVS-lib-cb.o
	gcc client.o KVS-lib.o KVS-lib-com.o KVS-lib-cb.o -o client
	
# ---------- LOCAL SERVER ----------
KVSLocalServer-data.o: KVSLocalServer-data.c KVSLocalServer-data.h KVSLocalServer-auth.h KVSLocalServer-base.h KVS-lib-MACROS.h
	gcc -c -g KVSLocalServer-data.c

KVSLocalServer-client.o: KVSLocalServer-client.c KVSLocalServer-client.h KVSLocalServer-data.h KVSLocalServer-com.h KVSLocalServer-cb.h KVSLocalServer-base.h KVS-lib-MACROS.h
	gcc -c -g KVSLocalServer-client.c

KVSLocalServer-com.o: KVSLocalServer-com.c KVSLocalServer-com.h KVSLocalServer-base.h KVS-lib-MACROS.h
	gcc -c -g KVSLocalServer-com.c

KVSLocalServer-auth.o: KVSLocalServer-auth.c KVSLocalServer-auth.h KVSLocalServer-base.h KVS-lib-MACROS.h
	gcc -c -g  KVSLocalServer-auth.c

KVSLocalServer-cb.o: KVSLocalServer-cb.c KVSLocalServer-cb.h KVSLocalServer-data.h KVSLocalServer-base.h KVS-lib-MACROS.h 
	gcc -c -g  KVSLocalServer-cb.c

ui.o: ui.c ui.h
	gcc -c -g ui.c

KVSLocalServer.o: KVSLocalServer.c KVSLocalServer.h KVSLocalServer-data.h KVSLocalServer-client.h KVSLocalServer-cb.h ui.h KVSLocalServer-base.h KVS-lib-MACROS.h 
	gcc -c -g KVSLocalServer.c

KVSLocalServer: KVSLocalServer.o KVSLocalServer-client.o KVSLocalServer-com.o KVSLocalServer-data.o KVSLocalServer-auth.o KVSLocalServer-cb.o ui.o
	gcc KVSLocalServer.o KVSLocalServer-com.o KVSLocalServer-client.o KVSLocalServer-data.o KVSLocalServer-auth.o KVSLocalServer-cb.o ui.o -o KVSLocalServer

clean:
	rm KVSLocalServer client *.o 