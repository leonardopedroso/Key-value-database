CFLAGS = -g -Wall

all: KVS-lib.a

KVS-lib-com.o: KVS-lib-com.c KVS-lib-com.h KVS-lib-base.h KVS-lib-MACROS.h
	gcc -c  $(CFLAGS) KVS-lib-com.c
KVS-lib-cb.o: KVS-lib-cb.c KVS-lib-cb.h KVS-lib-base.h KVS-lib-MACROS.h
	gcc -c  $(CFLAGS) KVS-lib-cb.c
KVS-lib.o: KVS-lib.c KVS-lib.h KVS-lib-com.c KVS-lib-cb.h KVS-lib-base.h KVS-lib-MACROS.h
	gcc -c  $(CFLAGS) KVS-lib.c
KVS-lib.a: KVS-lib-com.o KVS-lib-cb.o KVS-lib.o
	ar r KVS-lib.a KVS-lib-com.o KVS-lib-cb.o KVS-lib.o