#ifndef KVS_LIB_CB_H
#define KVS_LIB_CB_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h> // to manipulate strings
#include <pthread.h> // to use threads
#include <sys/socket.h> // to use sockets
#include <sys/un.h> // to use unix domain sockets
#include <signal.h> // to manage signals for client disconnection

#define MAX_LEN_CN_SERVER_ADDR 100
#define CB_SERVER_BACKLOG 1

#define DEBUG_CALLBACK 

typedef struct callbackStruct{
    int cb_id;
    void (*cb_func)(char *);

    struct callbackStruct * prox;
}CALLBACK;

void * callbackServerThread(void * arg);

void callbackDisconnect();
void callbackClear();

#endif