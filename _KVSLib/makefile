CFLAGS = -g -Wall
all: KVS-lib.so client

# ---------- KVS-LIB ----------
KVS-lib-com.o: KVS-lib-com.c KVS-lib-com.h KVS-lib-base.h KVS-lib-MACROS.h
	gcc -c  $(CFLAGS) KVS-lib-com.c
KVS-lib-cb.o: KVS-lib-cb.c KVS-lib-cb.h KVS-lib-base.h KVS-lib-MACROS.h
	gcc -c  $(CFLAGS) KVS-lib-cb.c
KVS-lib.o: KVS-lib.c KVS-lib.h KVS-lib-com.c KVS-lib-cb.h KVS-lib-base.h KVS-lib-MACROS.h
	gcc -c  $(CFLAGS) KVS-lib.c
KVS-lib.so: KVS-lib-com.o KVS-lib-cb.o KVS-lib.o
	gcc KVS-lib.o KVS-lib-com.o KVS-lib-cb.o -o KVS-lib.so -ldl -shared -fPIC

# ---------- CLIENT ----------
client.o: client.c
	gcc -c  $(CFLAGS) client.c

client: client.o KVS-lib.o KVS-lib-com.o KVS-lib-cb.o
	gcc client.o KVS-lib.o KVS-lib-com.o KVS-lib-cb.o -o client
	
clean:
	rm client *.o 