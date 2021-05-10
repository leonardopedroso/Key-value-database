#include <stdlib.h>
#include <stdio.h>
#include <string.h> // to manipulate strings
#include <pthread.h> // to use threads
#include <sys/socket.h> // to use sockets
#include <sys/un.h> // to use unix domain sockets
#include <signal.h> // to manage signals for client disconnection

#include "KVSLocalServer-data.h" // Data management functions
#include "KVSLocalServer-client.h" // Client management functions
#include "ui.h" // User interface
#include "KVS-lib-MACROS.h" // Shared MACROS

// Define server parameters
#define KVS_LOCAL_SERVER_ADDR "/tmp/KVSLocalServer"
#define KVS_LOCAL_SERVER_BACKLOG 5

// ---------- KVS Server thread prototypes ----------
void * KVSLocalServerThread(void * server_sock);
void * KVSLocalServerClientThread(void * clientSocket);