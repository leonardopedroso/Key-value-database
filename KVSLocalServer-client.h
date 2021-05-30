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

// ---------- Server and client management prototypes ----------
#define SUCCESS_CLIENT_HANDLE 0
#define ERROR_CLIENT_ALLOCATION -1
int clientHandle(int sockClient);

void clientAdd(CLIENT * client);

int clientAuth(CLIENT * client, char * groupId, char * secret);

int clientDisconnect(CLIENT * client);

int clientShow();

void closeClients();


#endif