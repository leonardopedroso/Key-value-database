#include "KVS-lib-cb.h"
#include "KVS-lib-com.h"

// ---------- Global variables ----------
#include "KVS-lib-cb.h"

// ---------- Global variables ----------
pthread_t cbThread; // Thread for the callback
CALLBACK * callbacks; // Pointer to list of callbacks
extern int cb_sock[2]; // callback socket
// [STATIC RW LOCK FOR CALLBACK LIST PROTECTION]

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
    char * value; // Allocate pointer to new value
    uint64_t valLen;
    int cb_id;
    while(1){
        value = NULL;
        // ---------- Receive callback
        if(rcvCallback(&cb_id, &value, &valLen) != RCV_CALLBACK_OK){
            #ifdef DEBUG_CALLBACK
            printf("Thread \n");
            #endif
            close(cb_sock[0]); // Close callback socket listening to connections
            cb_sock[0] = -1;
            close(cb_sock[1]);
            cb_sock[1] = -1;
            char cb_server_addr[MAX_LEN_CN_SERVER_ADDR];
            sprintf(cb_server_addr,"tmp/cb%d",getpid());
            remove(cb_server_addr); // Remove address
            callbackClear();
            pthread_exit(NULL);
        }
        // --------- Search callback

        // Synch?
        CALLBACK * searchPointer = callbacks;
        while(searchPointer != NULL){
            // Check if group already exists
            if(searchPointer->cb_id == cb_id){
                break;
            }
            searchPointer = searchPointer->prox;
        }
        if (searchPointer == NULL){
            // Loss of synch
            // Pensar nisto
        }else{
            // Run callback function
            (*(searchPointer->cb_func))(value);
        }
        // Synch?

        // ---------- Free memory
        free(value);
    }
    
    pthread_exit(NULL);
}


void callbackDisconnect(){
    close(cb_sock[0]); // Close callback socket listening to connections
    cb_sock[0] = -1;
    close(cb_sock[1]); // Close callback socket listening to connections
    cb_sock[1] = -1;
    pthread_join(cbThread,NULL); // Wait for server thread to quit
}

void callbackClear(){
    // Allocate pointer to group list
    CALLBACK * prev = NULL;
    // Synch?
    CALLBACK * searchPointer = callbacks;
    // Iterate through all groups
    while(searchPointer != NULL){
        // Check next element on the list
        prev = searchPointer;
        searchPointer = searchPointer->prox;
        // No need to delete client acess to group because the handler thread has 
        // already been joined when this function runs
        free(prev); // delete group block
    }
    callbacks = NULL;
    // Synch?
    #ifdef DEBUG_CALLBACK
    printf("All callbacks cleared.\n");
    #endif
}