all: client KVSLocalServer

# ---------- KVS-LIB ----------

KVS-lib-com.o: KVS-lib-com.c KVS-lib-com.h KVS-lib-MACROS.h
	gcc -c -g KVS-lib-com.c
KVS-lib.o: KVS-lib.c KVS-lib.h KVS-lib-com.c KVS-lib-com.h
	gcc -c -g KVS-lib.c

# ---------- CLIENT ----------

client.o: client.c
	gcc -c -g client.c

client: client.o KVS-lib.o KVS-lib-com.o
	gcc client.o KVS-lib.o KVS-lib-com.o -o client
	
# ---------- LOCAL SERVER ----------
KVSLocalServer-data.o: KVSLocalServer-data.c KVSLocalServer-data.h KVSLocalServer-client.h KVS-lib-MACROS.h
	gcc -c -g KVSLocalServer-data.c

KVSLocalServer-client.o: KVSLocalServer-client.c KVSLocalServer-client.h KVSLocalServer-data.h KVS-lib-MACROS.h
	gcc -c -g KVSLocalServer-client.c

KVSLocalServer-com.o: KVSLocalServer-com.c KVSLocalServer-com.h KVS-lib-MACROS.h
	gcc -c -g KVSLocalServer-com.c

KVSLocalServer-auth.o: KVSLocalServer-auth.c KVSLocalServer-auth.h KVS-lib-MACROS.h
	gcc -c -g  KVSLocalServer-auth.c

ui.o: ui.c ui.h
	gcc -c -g ui.c

KVSLocalServer.o: KVSLocalServer.c KVSLocalServer.h KVSLocalServer-data.h KVSLocalServer-client.h KVS-lib-MACROS.h 
	gcc -c -g KVSLocalServer.c

KVSLocalServer: KVSLocalServer.o KVSLocalServer-client.o KVSLocalServer-com.o KVSLocalServer-data.o KVSLocalServer-auth.o ui.o
	gcc KVSLocalServer.o KVSLocalServer-com.o KVSLocalServer-client.o KVSLocalServer-data.o KVSLocalServer-auth.o ui.o -o KVSLocalServer

clean:
	rm KVSLocalServer client *.o 