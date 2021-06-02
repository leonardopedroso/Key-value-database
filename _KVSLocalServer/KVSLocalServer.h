#ifndef KVS_LOCAL_SERVER_H
#define KVS_LOCAL_SERVER_H

#include "KVSLocalServer-base.h"

// Define server parameters
#define KVS_LOCAL_SERVER_BACKLOG 5

// ---------- KVS Server thread prototypes ----------
void * KVSLocalServerThread(void * server_sock);
void * KVSLocalServerClientThread(void * clientSocket);

// Define shuddown descriptors 
#define SD_CONTROLLED 1
#define SD_ACCEPT_ERROR 2
#define SD_ALLOCATION_ERROR 3
#define SD_SD_PIPE_ERROR 4
void * KVSLocalServerShutdownThread(void * arg);

#endif