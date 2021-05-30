#include "KVSLocalServer-client.h"

// ---------- Global variables ----------
CLIENT * clients = NULL; // Pointer to the first element of the linked list of clients 
static pthread_mutex_t clients_mtx = PTHREAD_MUTEX_INITIALIZER; // Mutex to protect client list
extern GROUP * groups; // Pointer to group list

void * KVSLocalServerClientThread(void * client){
    // Allocate buffers
    char * buffer1;
    char * buffer2;
    uint64_t buffer2Len;
    int msgId = 0;
    
    // Loop receiving and handling queries
    while(1){
        buffer1 = NULL;
        buffer2 = NULL;
        if(rcvQueryKVSLocalServer(((CLIENT *)client)->clientSocket, &msgId, &buffer1, &buffer2, &buffer2Len) == RCV_QUERY_COM_ERROR){
            // [CUIDADO QUANDO TIVER O CALLBACK]
            fprintf(stderr,"Uncommanded disconnection of PID: %d\n",((CLIENT *)client)->PID);
            ((CLIENT *)client)->connectivityStatus = CONN_STATUS_DISCONNECTED;
            close(((CLIENT *)client)->clientSocket);
            pthread_exit(NULL); // Close KVSServerThread
        }
        // ---------- Authenticate client ----------
        // - Establish connection
        if(msgId >= MSG_ID_ESTBL_CONN){ // PIDS greater than MSG_ID_ESTBL_CONN
            // Define client PID
            ((CLIENT *)client)->PID = msgId;
            // Output status to msgId just to avoid allocating another variable
            msgId = clientAuth(((CLIENT *)client), buffer1, buffer2);
            // Answer query of the client
            ansQueryKVSLocalServer(((CLIENT *)client)->clientSocket,msgId,NULL,0);
            if(msgId == STATUS_OK){
                printf("Client authenticated -> Group id: %s | Secret: %s | PID: %d\n", buffer1,buffer2,((CLIENT *)client)->PID);
            }
            // Free memory allocated by the reception of the query
            free(buffer1);
            free(buffer2);
            continue;
        }
        switch(msgId){
            case MSG_ID_PUT_VAL:
                // Output status to msgId just to avoid allocating another variable
                // Cannot send group poiter as argument beacause it may loose validy 
                // Group acess as to be check inside read lock entries
                //printf("Put value | key: %s | value %s\n",buffer1,buffer2);
                msgId = groupAddEntry((CLIENT *) client,buffer1,buffer2);
                ansQueryKVSLocalServer(((CLIENT *)client)->clientSocket,msgId,NULL,0);
                // Memory on buffer1 and buffer2 is not freed because it is used as the memory allocated for the key value pair
                // On error buffer1 and buffer2 are freed inside groupAddEntry
                break;
            case MSG_ID_GET_VAL:{
                // Output status to msgId just to avoid allocating another variable
                // Cannot send group poiter as argument beacause it may loose validy 
                // Group acess as to be check inside read lock entries
                free(buffer2);
                char * val = NULL;
                uint64_t valLen = 0;
                //printf("Read value | key: %s \n",buffer1);
                msgId = groupReadEntry((CLIENT *) client,buffer1,&val,&valLen);
                //printf("Read value | value: %s | len: %llu \n",val,valLen);
                ansQueryKVSLocalServer(((CLIENT *)client)->clientSocket,msgId,val,valLen);
                free(val);
                free(buffer1);
                break;
            }
            case MSG_ID_DEL_VAL:
                // Output status to msgId just to avoid allocating another variable
                // Cannot send group poiter as argument beacause it may loose validy 
                // Group acess as to be check inside read lock entries
                free(buffer2);
                msgId = groupDeleteEntry((CLIENT *) client,buffer1);
                ansQueryKVSLocalServer(((CLIENT *)client)->clientSocket,msgId,NULL,0);
                free(buffer1);
                break;
        
                break;
            case MSG_ID_REG_CB:
        
                break;
            // ---------- Close client connection----------
            case MSG_ID_CLOSE_CONN:
                // Commanded disconnection
                msgId = clientDisconnect(client);
                ansQueryKVSLocalServer(((CLIENT *)client)->clientSocket,msgId,NULL,0);
                // [CUIDADO QUANDO TIVER O CALLBACK]
                close(((CLIENT *)client)->clientSocket);
                // Free memory allocated in this query (note that buffer2 may be NULL)
                // It is only guaranteed that buffer2 does not have an invalid address
                free(buffer1);
                free(buffer2);
                pthread_exit(NULL); // Close KVSServerThread
                break;
            default:
                printf("Received something strange.\n");
                // Free memory allocated in this query (note that buffer2 may be NULL)
                // It is only guaranteed that buffer2 does not have an invalid address
                free(buffer1);
                free(buffer2);
                break;
        }
       
        
        
    }
}

// ---------- Server and client mangement prototypes ----------

int clientHandle(int clientSocket){
    // ---------- Allocate memory to new client ----------
    CLIENT * newClient = (CLIENT *) malloc(sizeof(CLIENT));
    // Catch allocation error 
    if(newClient == NULL){
        return ERROR_CLIENT_ALLOCATION;
    }
    // Init block
    newClient->prox = NULL;
    newClient->authGroup = NULL;
    // Store socket for communication with this client
    newClient->clientSocket = clientSocket;
    // Define connectivity status of client
    newClient->connectivityStatus = CONN_STATUS_NOT_AUTH;
    // Define connection time
    if(clock_gettime(CLOCK_REALTIME, &(newClient->connTime)) == -1 ) {
        perror("Clock gettime error");
        // Time is not critical so exit is overkill (exit commented out below)
        // return ERROR_CLIENT_CLOCK
    }
    // Add client to client list
    int clientAddStatus = clientAdd(newClient);
    if (clientAddStatus != SUCCESS_CLIENT_HANDLE){
        free(newClient);
        return ERROR_CLIENT_MUX;
    }
    // ---------- Handle client in new thread ----------
    pthread_create(&(newClient->clientThread), NULL, &KVSLocalServerClientThread, (void *) newClient);
    return SUCCESS_CLIENT_HANDLE;
}

int clientAdd(CLIENT * client){
    
    // ---------- Add new client block to the linked list ----------
    int status_mux;
    // [IN MUTEX client region]
    status_mux = pthread_mutex_lock(&clients_mtx);
    if(status_mux != 0){
        return ERROR_CLIENT_MUX;
    }
    CLIENT * searchPointer = clients;
    // Check if pointer to the linked list is NULL (i.e. the new client is the first client)
    if(searchPointer == NULL){
        clients = client;
    }else{
        // Iterate through the clients until the last, whcih does not point to other CLIENT block
        while(searchPointer->prox != NULL){
            searchPointer = searchPointer->prox;
        }
        searchPointer->prox = client;
    }
    pthread_mutex_unlock(&clients_mtx);
    // [OUT MUTEX client region]
    return SUCCESS_CLIENT_HANDLE;
}

int clientAuth(CLIENT * client, char * groupId, char * secret){
    // ---------- Verify if group exists ----------
    // Working in the whole group list is vulnerable to synch problems
    // [READ LOCK groups]
    // Allocate pointer to group list
    GROUP * searchPointer = groups;
    // Find group
    while(1){
        // If end of the list is reached without finding the group
        if(searchPointer == NULL){
            return STATUS_GROUP_DSN_EXIST;
        }
        // Check until group is found
        if(strcmp(searchPointer->id,groupId) == 0){
            break;
        }
        // Check next element on the list
        searchPointer = searchPointer->prox;
    }
    // [READ UNLOCK groups]
    // ----------- Authenticate secret ----------
    // [CHECK AUTHENTICATION AUTH SERVER]
    // switch(authenticated)
    // case OK:
        client->connectivityStatus = CONN_STATUS_CONNECTED;
        // return query status
        
    // ----------- Authenticate secret ----------
    client->authGroup = searchPointer;
    return STATUS_OK;
}

int clientDisconnect(CLIENT * client){
    // Set connectivity status
    client->connectivityStatus = CONN_STATUS_DISCONNECTED;
    // Define disconnection time
    if(clock_gettime(CLOCK_REALTIME, &(client->connTime)) == -1 ) {
        perror("Clock gettime error");
        // Time is not critical so exit is overkill
    }
    client->authGroup = NULL; // Remove access to group 
    printf("Commanded disconnection of PID: %d\n",client->PID);
    return STATUS_OK;
}

int clientShow(){
    // Buffer and struct for time string conversion
    char buf[50];
    struct tm info;
    printf("Client list:\n");
    // Differentiate between connected and disconnected clients
    int flagConnectivity = CONN_STATUS_CONNECTED;
    // [IN MUTEX client region]
    pthread_mutex_lock(&clients_mtx);
    CLIENT * searchPointer = clients;
    // Iterate through the clients
    printf("---------- Connected clients ----------\n");
    while(searchPointer != NULL){
        if(searchPointer->connectivityStatus == flagConnectivity){
            printf("PID: %d | ",searchPointer->PID);
            localtime_r(&(searchPointer->connTime.tv_sec),&info);
            if(flagConnectivity == CONN_STATUS_CONNECTED || flagConnectivity == CONN_STATUS_NOT_AUTH){
                printf("Connected at: %s", asctime_r(&info,buf));
            }else{
                printf("Disconnected at: %s", asctime_r(&info,buf));
            }
        }
        // Next element on the list 
        searchPointer = searchPointer->prox;
        fflush(stdout);
        if(searchPointer == NULL){
            switch (flagConnectivity){
            case CONN_STATUS_CONNECTED:
                flagConnectivity = CONN_STATUS_NOT_AUTH;
                printf("---------- Clients waiting authentication ----------\n");
                searchPointer = clients;
                break;
            case CONN_STATUS_NOT_AUTH:
                flagConnectivity = CONN_STATUS_DISCONNECTED;
                printf("---------- Disconnected clients ----------\n");
                searchPointer = clients;
                break;
            default:
                break;
            }
        }
    }
    pthread_mutex_unlock(&clients_mtx);
    // [OUT MUTEX client region]
    return 0;
}

void closeClients(){
    CLIENT * searchPointerPrev;
    // [IN MUTEX client region]
    pthread_mutex_lock(&clients_mtx);
    // ---------- Remove all client blocks of the linked list ----------
    CLIENT * searchPointer = clients;
    // Iterate through the clients closing, joining, and freeing memory
    while(searchPointer != NULL){
        // Next element on the list
        searchPointerPrev = searchPointer;
        searchPointer = searchPointer->prox;
        close(searchPointerPrev->clientSocket); // Close socket 
        pthread_join(searchPointerPrev->clientThread, NULL); // Wait for client thread
        free(searchPointerPrev); // Free memory allocated for client
    }
    pthread_mutex_unlock(&clients_mtx);
    // [OUT MUTEX client region]
    printf("Connection to all clients was terminated.\n");
}

void clientDeleteAccessGroup(GROUP * groupPtr){
    pthread_mutex_lock(&clients_mtx);
    // ---------- Disable all client blocks of the linked list that have access to  ----------
    CLIENT * searchPointer = clients;
    // Iterate through the clients closing, joining, and freeing memory
    while(searchPointer != NULL){
        // Check if this client had access the the group recently eliminated
        if (searchPointer->authGroup == groupPtr){
            // Clould close socket and pthread_wait, but it is excessive
            // close(searchPointer->clientSocket); // Close socket 
            // pthread_join(searchPointer->clientThread, NULL); // Wait for client thread
            // Set connectivity status
            searchPointer->connectivityStatus = CONN_STATUS_NOT_AUTH;
            // Clear group access  
            // !!!!!!!! MAYBE INTERRUPT MUTEX client region here]
            // [MUTEX IN AuthGroup]
            searchPointer->authGroup = NULL;
            // [MUTEX OUT AuthGroup]
            // Define disconnection time
            if(clock_gettime(CLOCK_REALTIME, &(searchPointer->connTime)) == -1 ) {
                perror("Clock gettime error");
                // Time is not critical so exit is overkill
            }
            printf("Application PID: %d waiting reauthentication : Authorized group no longer valid.\n",searchPointer->PID);
        }else{
            // Next element on the list 
            searchPointer = searchPointer->prox;
        }
    }
    pthread_mutex_unlock(&clients_mtx);
    // [OUT MUTEX client region]
}