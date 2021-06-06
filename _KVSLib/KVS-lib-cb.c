#include "KVS-lib-base.h"
#include "KVS-lib-cb.h"

// ---------- Global variables ----------
pthread_t cbThread; // Thread for the callback
CALLBACK * callbacks = NULL; // Pointer to list of callbacks
static pthread_rwlock_t callbacks_rwlock = PTHREAD_RWLOCK_INITIALIZER; // RW lock to protect callback list
extern int cb_sock[2]; // callback socket


void * callbackServerThread(void * arg){
    // Wait for the connection of the KVS local server
    cb_sock[1] = accept(cb_sock[0], NULL, NULL);
    // Catch error waiting for a connection
    if(cb_sock[1] == -1){
        close(cb_sock[0]);
        cb_sock[0] = -1;
        pthread_exit(NULL);
    }
    #ifdef DEBUG_CALLBACK
    printf("Callback server connected to KVS local server.\n");
    #endif

    // Wait for callbacks for KVS local server
    while(1){
        // ---------- Receive callback
        int * cb_id = malloc(sizeof(int));
        if(read(cb_sock[1], cb_id, sizeof(int)) <= 0){
            #ifdef DEBUG_CALLBACK
            printf("Callback server thread is exiting.\n");
            #endif
            close(cb_sock[0]); // Close callback socket listening to connections
            cb_sock[0] = DISCONNECTED_SOCKET;
            close(cb_sock[1]);
            cb_sock[1] = DISCONNECTED_SOCKET;
            free(cb_id); // Free callback id
            char cb_server_addr[MAX_LEN_CN_SERVER_ADDR];
            sprintf(cb_server_addr,"/tmp/cb%d",getpid());
            remove(cb_server_addr); // Remove address
            callbackClear();
            pthread_exit(NULL);
        }
        #ifdef DEBUG_CALLBACK
        printf("Received callback flag: %d.\n", *cb_id);
        #endif
        // --------- Handle callback
        pthread_t callbackExec;
        pthread_create(&callbackExec,NULL,&callbackWrapperThread,cb_id);
    }
    pthread_exit(NULL);
}

int callbackAdd(char * key, void (*callback_function)(char *)){
    // ---------- Allocate callback ----------
    CALLBACK * newCallback = (CALLBACK *) malloc(sizeof(CALLBACK));
    if (newCallback == NULL){
        return CALLBACK_ALLOC_ERROR;
    }
    newCallback->prox = NULL;
    newCallback->key = (char * ) malloc(strlen(key)+1);
    if(newCallback->key == NULL){
        free(newCallback);
        return CALLBACK_ALLOC_ERROR;
    }
    strcpy(newCallback->key,key);
    newCallback->cb_func = callback_function;
    
    // ---------- Search callback and assign cb_id ----------
    int cb_id = 0;
    // [WRITE LOCK CALLBACKS]
    pthread_rwlock_wrlock(&callbacks_rwlock);
    CALLBACK * searchPointer = callbacks;
    if(callbacks == NULL){
        callbacks = newCallback;
    }else{
        cb_id++; // Increase cb id
        while(searchPointer->prox != NULL){
            searchPointer = searchPointer->prox;
            cb_id++;
        }
        newCallback->cb_id = cb_id;
        searchPointer->prox = newCallback;
    }
    // [UNLOCK CALLBACKS]
    pthread_rwlock_unlock(&callbacks_rwlock);
    #ifdef DEBUG_CALLBACK
    printf("Created callback block id: %d.\n",cb_id);
    #endif
    return cb_id;
}

void * callbackWrapperThread(void * arg){
    // ---------- Search callback ----------
    // [READ LOCK CALLBACKS]
    pthread_rwlock_rdlock(&callbacks_rwlock);
    CALLBACK * searchPointer = callbacks;
    while(searchPointer != NULL){
        // Check if group already exists
        if(searchPointer->cb_id == *((int *)arg)){
            break;
        }
        searchPointer = searchPointer->prox;
    }
    if (searchPointer == NULL){
        pthread_rwlock_unlock(&callbacks_rwlock);
        pthread_exit(NULL);
        // Loss of synch
        // Callbacks were cleared connection was closed
    }else{
        // Run callback function
        char * cpkey = (char *) malloc(strlen(searchPointer->key)+1);
        if (cpkey == NULL){
            pthread_rwlock_unlock(&callbacks_rwlock);
            fprintf(stderr,"Memory allocation for callback function failed.\n");
            pthread_exit(NULL);
        }
        strcpy(cpkey,searchPointer->key);
        void (*cpfunc)(char *) = searchPointer->cb_func;
        pthread_rwlock_unlock(&callbacks_rwlock);
        // [READ UNLOCK CALLBACKS]
        // Run callback function
        // Note that this function is not dependent on memory allocated by the connection
        // Thus, the connection can be terminated, callback memory deallocated, that this functions keeps running
        #ifdef DEBUG_CALLBACK
        printf("Running callback id: %d.\n",*((int *)arg));
        #endif
        (*cpfunc)(cpkey);
        free(cpkey);
    }
    // Free memory
    free((int *)arg);
    pthread_exit(NULL);
}

void callbackDisconnect(){
    shutdown(cb_sock[0],2);
    close(cb_sock[0]); // Close callback socket listening to connections
    close(cb_sock[1]); // Close callback socket listening to connections
    #ifdef DEBUG_CALLBACK
    printf("Callback socket closed.\n");
    #endif
    pthread_join(cbThread,NULL); // Wait for server thread to quit
}

void callbackClear(){
    // Allocate pointer to group list
    CALLBACK * prev = NULL;
    // [WRITE LOCK CALLBACKS]
    pthread_rwlock_wrlock(&callbacks_rwlock);
    CALLBACK * searchPointer = callbacks;
    // Remove access to the list 
    callbacks = NULL;
    pthread_rwlock_unlock(&callbacks_rwlock);
    // [UNLOCK CALLBACKS]
    // Iterate through all groups
    while(searchPointer != NULL){
        // Check next element on the list
        prev = searchPointer;
        searchPointer = searchPointer->prox;
        // No need to delete client acess to group because the handler thread has 
        // already been joined when this function runs
        free(prev->key);
        free(prev); // delete group block
    }
    #ifdef DEBUG_CALLBACK
    printf("All callbacks cleared.\n");
    #endif
}