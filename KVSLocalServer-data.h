#ifndef KVS_LOCAL_SERVER_DATA_H
#define KVS_LOCAL_SERVER_DATA_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h> // to manipulate strings
#include <pthread.h> // to use threads
#include <time.h>
#include "KVS-lib-MACROS.h"
#include "KVSLocalServer-auth.h"


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
    //number of entries as variable instead of counting each time this values is needed
    // If one has to cout each time it would hacve to be inside mutexs, because is is 
    // accessed by pointers that can become unvalid after entry remove
    int numberEntries; 

    struct groupStruct * prox;
}GROUP;

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

// ---------- Data management prototypes ----------
#define GROUP_OK 0
#define GROUP_ALREADY_EXISTS -1
#define GROUP_DSNT_EXIST -2
#define GROUP_ALLOC_ERROR -3

#define GROUP_AUTH_COM_ERROR -100
#define GROUP_LOSS_SYNCH -101


int groupAdd(char * group);
int groupDelete(char * group);
int groupShow(char * group);
void groupClear();

int groupAddEntry(CLIENT * client, char * key, char * value);
int groupReadEntry(CLIENT * client, char * key, char ** val, uint64_t * valLen);
int groupDeleteEntry(CLIENT * client, char * key);

// ---------- Auxiliary functions ----------
void entriesDelete(GROUP * group);

// ----------- Client - data interaction ----
void clientDeleteAccessGroup(GROUP * groupPtr);

#endif