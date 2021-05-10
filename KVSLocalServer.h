#include <stdlib.h>
#include <stdio.h>
#include <string.h> // to manipulate strings
#include <unistd.h> // to open, close, read
#include <pthread.h> // to use threads
#include <sys/socket.h> // to use sockets
#include <sys/un.h> // to use unix domain sockets
#include <signal.h> // to manage signals for client disconnection
#include <time.h> // to get connection and disconnection time

// Define server parameters
#define KVS_LOCAL_SERVER_ADDR "/tmp/KVSLocalServer"
#define KVS_LOCAL_SERVER_BACKLOG 5

// Struct to hold clients
typedef struct clientStruct{
    int clientSocket;
    pthread_t clientThread;
    struct timespec connTime; // -> to date with struct tm *my_tm = localtime(&ts.tv_sec);
    int connectivityStatus;
    int PID;
    
    struct clientStruct * prox;
}CLIENT;
#define CONN_STATUS_CONNECTED 0
#define CONN_STATUS_DISCONNECTED 1
#define CONN_STATUS_NOT_AUTH 3

// ---------- KVS Server thread prototypes ----------
void * KVSLocalServerThread(void * server_sock);
void * KVSLocalServerClientThread(void * clientSocket);

// ---------- Server and client management prototypes ----------
#define SUCCESS_CLIENT_HANDLE 0
#define ERROR_CLIENT_ALLOCATION -1
int handleClient(int sockClient);

void clientAdd(CLIENT * newclient);
// [IMPLEMENT manageClients to avoid synch problems]
// [TODAS AS ACOES QUE ALTEREM A MEMORIA DA LISTA DE CLIENTES TEM DE PASSAR PELA FUNÇAO manageClients]
//void manageClients(int action,...)
void closeClient(CLIENT * client);
void closeClients();

// ---------- Data management prototypes ----------

// Struct to hold key-value pairs
typedef struct entryStruct{
    char * key; //with malloc instead of char key[MAX_STR_LENGTH];
    char * value; //with malloc instead of char value[MAX_STR_LENGTH];

    struct entryStruct * prox;
}ENTRY;

// Struct to hold 
typedef struct groupStruct{
    char * id; //with malloc instead of char id[MAX_STR_LENGTH];
    ENTRY * entries;

    struct groupStruct * prox;
}GROUP;

// [IMPLEMENT manageData to avoid synch problems]
// [TODAS AS ACOES QUE ALTEREM A MEMORIA TEM DE PASSAR PELA FUNÇAO manageData]
//void manageData(int action,...)
