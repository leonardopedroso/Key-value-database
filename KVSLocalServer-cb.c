#include "KVSLocalServer-cb.h"

// ---------- Global variables ----------
CALLBACK * callbacks = NULL;
// Static rw lock

void callbackConnect(CLIENT * client){
    // ---------- Setup server connection ----------
    // Create client socket    
    client->cb_sock = socket(AF_UNIX,SOCK_STREAM,0);
    // Ignore socket disconnection signal from client, which is handled when read/write returns -1
    signal(SIGPIPE, SIG_IGN); 
    // Catch errors creating socket
    if (client->cb_sock == DISCONNECTED_SOCKET){
        fprintf(stderr,"Error creating callbak socket for client PID: %d.\n",client->PID);
        return;
    }
    #ifdef DEBUG_CALLBACK
    printf("Created callback socket of client PID: %d.\n",client->PID);
    #endif
    // Setup server connection
    struct sockaddr_un cb_sock_addr;
    cb_sock_addr.sun_family = AF_UNIX;
    char cb_server_addr[MAX_LEN_CN_SERVER_ADDR];
    sprintf(cb_server_addr,"/tmp/cb%d",client->PID);
    strcpy(cb_sock_addr.sun_path, cb_server_addr);
    if(connect(client->cb_sock, (struct sockaddr *) &cb_sock_addr, sizeof(struct sockaddr_un)) == -1){
        fprintf(stderr,"Error connecting to callbak socket for client PID: %d.\n",client->PID);
        close(client->cb_sock);
        client->cb_sock = DISCONNECTED_SOCKET;
        return;
    }
    #ifdef DEBUG_CALLBACK
    printf("Connected to callback socket of client PID: %d.\n",client->PID);
    #endif
}

int callbackRegister(CLIENT* client, char * key, char * cb_id){
    // ---------- Check authentication and if client has access to that key
    // [MUTEX AuthGroup]
    pthread_mutex_lock(&client->authGroup_mtx);
    // 1. Check if the authorized group address is valid 
    if(client->connectivityStatus != CONN_STATUS_CONNECTED || client->authGroup == NULL){
        pthread_mutex_unlock(&client->authGroup_mtx);
        // [READ UNLOCK AuthClient]
        // Free memory on error
        free(key);
        #ifdef DEBUG_CALLBACK
        printf("Unauthorized to register callback of client PID: %d.\n",client->PID);
        #endif
        return STATUS_ACCSS_DENIED;
    }
    pthread_mutex_unlock(&client->authGroup_mtx);
    // 2. Check if key exists
    ENTRY * prev = NULL;
    // [WRITE LOCK ENTRIES] 
    pthread_rwlock_rdlock(&client->authGroup->entries_rwlock);
    ENTRY * searchEntry = client->authGroup->entries;
    while(1){
        // If end of the list is reached
        if (searchEntry == NULL){
            free(key);
            #ifdef DEBUG_CALLBACK
            printf("Key not found to register callback of client PID: %d.\n",client->PID);
            #endif
            pthread_rwlock_unlock(&client->authGroup->entries_rwlock);
            return STATUS_KEY_DSNT_EXIST;
        }
        // If key is found
        if(strcmp(searchEntry->key,key)==0){
            pthread_rwlock_unlock(&client->authGroup->entries_rwlock);
            break;
        }
        searchEntry = searchEntry->prox;
    }
    // ---------- Create and add callback
    CALLBACK * newCallback = (CALLBACK * ) malloc(sizeof(CALLBACK));
    if (newCallback == NULL){
        free(key);
        return STATUS_ALLOC_ERROR;
    }
    newCallback->prox = NULL;
    newCallback->cb_sock = client->cb_sock;
    newCallback->key = key;
    newCallback->cb_id = *((int *) cb_id);
    // [WRITE LOCK CALLBACKS]
    CALLBACK * searchPointer = callbacks;
    if(callbacks == NULL){
        callbacks = newCallback;
    }else{
        while(searchPointer->prox != NULL){
            searchPointer = searchPointer->prox;
        }
        searchPointer->prox = newCallback;
    }
    // [UNLOCK CALLBACKS]
    #ifdef DEBUG_CALLBACK
    printf("Registered callback id: %d of client PID: %d.\n",newCallback->cb_id, client->PID);
    #endif
    return STATUS_OK;
}

void callbackDeleteKey(char * key){
    CALLBACK * prev = NULL;
    // [WRITE LOCK CALLBACKS]
    CALLBACK * searchPointer = callbacks;
    while(searchPointer != NULL){
        // Find callbacks with key
        if(strcmp(searchPointer->key,key) ==0){
            if(prev == NULL){
                callbacks = searchPointer->prox;
            }else{
                prev->prox = searchPointer->prox;
            }
            // Free memory
            free(searchPointer->key);
            free(searchPointer);
        }
        prev = searchPointer;
        searchPointer = searchPointer->prox;
    }
    // [UNLOCK CALLBACKS]
    #ifdef DEBUG_CALLBACK
    printf("Cleared callbacks with key: %s.\n",key);
    #endif
}

void callbackDeleteClient(int cb_sock){
    CALLBACK * prev = NULL;
    // [WRITE LOCK CALLBACKS]
    CALLBACK * searchPointer = callbacks;
    while(searchPointer != NULL){
        // Find callbacks with key
        if(searchPointer->cb_sock == cb_sock){
            if(prev == NULL){
                callbacks = searchPointer->prox;
            }else{
                prev->prox = searchPointer->prox;
            }
            CALLBACK * aux = searchPointer;
            searchPointer = searchPointer->prox;
            // Free memory
            free(aux->key);
            free(aux);
            continue;
        }
        prev = searchPointer;
        searchPointer = searchPointer->prox;
    }
    // [UNLOCK CALLBACKS]
    #ifdef DEBUG_CALLBACK
    printf("Cleared callbacks with callback socket: %d.\n",cb_sock);
    #endif
}

void callbackFlag(char * key){
    CALLBACK * prev = NULL;
    // [READ LOCK CALLBACKS]
    CALLBACK * searchPointer = callbacks;
    while(searchPointer != NULL){
        // Find callbacks with key
        if(strcmp(searchPointer->key,key) ==0){
            if(write(searchPointer->cb_sock,&searchPointer->cb_id,sizeof(int))<=0){
                // Nothing more is done because it would require a write lock
                // A write lock is too strong for a function executed so many times
                close(searchPointer->cb_sock);
            }
        }
        prev = searchPointer;
        searchPointer = searchPointer->prox;
    }
    // [UNLOCK CALLBACKS]
    #ifdef DEBUG_CALLBACK
    printf("Flagged callbacks with key: %s.\n",key);
    #endif
}