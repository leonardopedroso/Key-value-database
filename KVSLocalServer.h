#include <stdlib.h>
#include <stdio.h>
#include <string.h> // to manipulate strings
#include <unistd.h> // to open, close, read
#include <pthread.h> // to use threads
#include <sys/socket.h> // to use sockets
#include <sys/un.h> // to use unix domain sockets
#include <signal.h> // to manage signals for client disconnection

// Define server parameters
#define KVS_LOCAL_SERVER_ADDR "/tmp/KVSLocalServer"
#define KVS_LOCAL_SERVER_BACKLOG 5

// Struct to hold clients
typedef struct clientStruct{
    int clientSocket;
    pthread_t clientThread;
    struct clientStruct * prox;
}CLIENT;

// ---------- KVS Server thread prototypes ----------
void * KVSLocalServerThread(void * server_sock);
void * KVSLocalServerClientThread(void * clientSocket);

// ---------- Server and client mangement prototypes ----------
#define SUCCESS_CLIENT_HANDLE 0
#define ERROR_CLIENT_ALLOCATION -1

int handleClient(int sockClient);
void closeClients();

// ---------- Data management ----------
// [POR ENQUANTO ESTAMOS A ETABELECER UM LIMITE DE CARACTERES MAS 
// PODE-SE NA BOA ALOCAR DINAMICAMENTE SEM LIMITE SE QUISERMOS DEPOIS]
// Data management parameters
#define KEY_MAX_LENGTH 20
#define VALUE_MAX_LENGTH 20
#define GROUP_ID_MAX_LENGTH 20
#define GROUP_SECRET_MAX_LENGTH 20

// Struct to hold key-value pairs
typedef struct entryStruct{
    char key[KEY_MAX_LENGTH];
    char value[KEY_MAX_LENGTH];

    struct entryStruct * prox;
}ENTRY;

// Struct to hold 
typedef struct groupStruct{
    char id[GROUP_ID_MAX_LENGTH];
    ENTRY * entries;

    struct groupStruct * prox;
}GROUP;

