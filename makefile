all: client KVSLocalServer

# ---------- KVS-LIB ----------

KVS-lib-com.o: KVS-lib-com.c KVS-lib-com.h KVS-lib-MACROS.h
	gcc -c KVS-lib-com.c
KVS-lib.o: KVS-lib.c KVS-lib.h KVS-lib-com.c KVS-lib-com.h
	gcc -c KVS-lib.c

# ---------- CLIENT ----------

client.o: client.c
	gcc -c client.c

client: client.o KVS-lib.o KVS-lib-com.o
	gcc client.o KVS-lib.o KVS-lib-com.o -o client
	
# ---------- LOCAL SERVER ----------

KVSLocalServer-com.o: KVSLocalServer-com.c KVSLocalServer-com.h KVS-lib-MACROS.h
	gcc -c KVSLocalServer-com.c

ui.o: ui.c ui.h
	gcc -c ui.c

KVSLocalServer.o: KVSLocalServer.c KVSLocalServer.h KVS-lib-MACROS.h
	gcc -c KVSLocalServer.c

KVSLocalServer: KVSLocalServer.o KVSLocalServer-com.o ui.o
	gcc KVSLocalServer.o KVSLocalServer-com.o ui.o -o KVSLocalServer
