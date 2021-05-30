#ifndef KVS_LOCAL_SERVER_CLIENT_H
#define KVS_LOCAL_SERVER_CLIENT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h> // to manipulate strings
#include <pthread.h> // to use threads
#include <time.h> // to get connection and disconnection time

#include "KVSLocalServer-data.h" // to manage data
#include "KVSLocalServer-com.h" // to communicate with KVS-lib
#include "KVS-lib-MACROS.h" // Shared MACROS

// Struct to hold clients
typedef struct clientStruct{
    int clientSocket;
    pthread_t clientThread;
    struct timespec connTime; // -> to date with struct tm *my_tm = localtime(&ts.tv_sec);
    int connectivityStatus;
    int PID;
    struct groupStruct * authGroup;
    
    struct clientStruct * prox;
}CLIENT;

// Connectivity status
#define CONN_STATUS_CONNECTED 0
#define CONN_STATUS_DISCONNECTED 1
#define CONN_STATUS_NOT_AUTH 3

// ---------- Server and client management prototypes ----------
#define SUCCESS_CLIENT_HANDLE 0
#define ERROR_CLIENT_ALLOCATION -1
int clientHandle(int sockClient);

void clientAdd(CLIENT * client);

int clientAuth(CLIENT * client, char * groupId, char * secret);

void clientDeleteAccessGroup(struct groupStruct * groupPtr);

int clientDisconnect(CLIENT * client);

int clientShow();

void closeClients();


#endif