#include "KVSLocalServer-client.h"
#include "KVSLocalServer-data.h"

// ---------- Global variables ----------
CLIENT * clients = NULL; // Pointer to the first element of the linked list of clients 
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
        
                break;
            case MSG_ID_GET_VAL:
        
                break;
            case MSG_ID_DEL_VAL:
        
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
                pthread_exit(NULL); // Close KVSServerThread
                break;
            default:
                break;
        }
        // Free memory allocated in this query (note that buffer2 may be NULL)
        // It is only guaranteed that buffer2 does not have an invalid address
        free(buffer1);
        free(buffer2);
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
    clientAdd(newClient);
    // ---------- Handle client in new thread ----------
    pthread_create(&(newClient->clientThread), NULL, &KVSLocalServerClientThread, (void *) newClient);
    return SUCCESS_CLIENT_HANDLE;
}

void clientAdd(CLIENT * client){
    // [IN MUTEX client region]
    // ---------- Add new client block to the linked list ----------
    // Check if pointer to the linked list is NULL (i.e. the new client is the first client)
    CLIENT * searchPointer = clients;
    if(searchPointer == NULL){
        clients = client;
    }else{
        // Iterate through the clients until the last, whcih does not point to other CLIENT block
        while(searchPointer->prox != NULL){
            searchPointer = searchPointer->prox;
        }
        searchPointer->prox = client;
    }
    // [OUT MUTEX client region]
}

int clientAuth(CLIENT * client, char * groupId, char * secret){
    // ---------- Verify if group exists ----------
    // Working in the whole group list is vulnerable to synch problems
    // [MUTEX IN region groups]
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
    // [MUTEX OUT region groups]
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
    // [IN MUTEX client region]
    CLIENT * searchPointer = clients;
    printf("Client list:\n");
    // Differentiate between connected and disconnected clients
    int flagConnectivity = CONN_STATUS_CONNECTED;
    // Iterate through the clients
    while(searchPointer != NULL){
        switch (flagConnectivity){
        case CONN_STATUS_CONNECTED:
            printf("---------- Connected clients ----------\n");
            break;
        case CONN_STATUS_NOT_AUTH:
            printf("---------- Clients waiting authentication ----------\n");
            break;
        case CONN_STATUS_DISCONNECTED:
            printf("---------- Disconnected clients ----------\n");
            break;
        }
        if(searchPointer->connectivityStatus == flagConnectivity){
            printf("PID: %d | ",searchPointer->PID);
            
            struct tm *info;
            info = localtime(&(searchPointer->connTime.tv_sec));
            if(flagConnectivity == CONN_STATUS_CONNECTED || flagConnectivity == CONN_STATUS_NOT_AUTH){
                printf("Connected at: %s", asctime(info));
            }else{
                printf("Disconnected at: %s", asctime(info));
            }
        }
        // Next element on the list 
        searchPointer = searchPointer->prox;
        if(searchPointer == NULL){
            switch (flagConnectivity){
            case CONN_STATUS_CONNECTED:
                flagConnectivity = CONN_STATUS_NOT_AUTH;
                searchPointer = clients;
                break;
            case CONN_STATUS_NOT_AUTH:
                flagConnectivity = CONN_STATUS_DISCONNECTED;
                searchPointer = clients;
                break;
            default:
                break;
            }
        }
    }
    // [OUT MUTEX client region]
    return 0;
}

void closeClients(){
    // [IN MUTEX client region]
    // ---------- Remove all client blocks of the linked list ----------
    CLIENT * searchPointer = clients;
    CLIENT * searchPointerPrev;

    // Iterate through the clients closing, joining, and freeing memory
    while(searchPointer != NULL){
        // Next element on the list
        searchPointerPrev = searchPointer;
        searchPointer = searchPointer->prox;
        close(searchPointerPrev->clientSocket); // Close socket 
        pthread_join(searchPointerPrev->clientThread, NULL); // Wait for client thread
        free(searchPointerPrev); // Free memory allocated for client
    }
    // [OUT MUTEX client region]
    printf("Connection to all clients was terminated.\n");
}

void clientDeleteAccessGroup(GROUP * groupPtr){
    // [IN MUTEX client region]
    // ---------- Disable all client blocks of the linked list that have access to  ----------
    CLIENT * searchPointer = clients;
    // Iterate through the clients closing, joining, and freeing memory
    while(searchPointer != NULL){
        // Check if this client had access the the group recently eliminated
        if (searchPointer->authGroup == groupPtr){
            close(searchPointer->clientSocket); // Close socket 
            pthread_join(searchPointer->clientThread, NULL); // Wait for client thread
            // Set connectivity status
            searchPointer->connectivityStatus = CONN_STATUS_DISCONNECTED;
            // Clear group access
            searchPointer->authGroup = NULL;
            // Define disconnection time
            if(clock_gettime(CLOCK_REALTIME, &(searchPointer->connTime)) == -1 ) {
                perror("Clock gettime error");
                // Time is not critical so exit is overkill
            }
            printf("Commanded disconnection of PID: %d : Authorized group no longer valid.\n",searchPointer->PID);
        }else{
            // Next element on the list 
            searchPointer = searchPointer->prox;
        }
    }
    // [OUT MUTEX client region]
}