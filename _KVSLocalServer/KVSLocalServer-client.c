#include "KVSLocalServer-client.h"
#include "KVSLocalServer-cb.h"
#include "KVSLocalServer-com.h" // to communicate with KVS-lib
#include "KVSLocalServer-auth.h"

// ---------- Global variables ----------
CLIENT * clients = NULL; // Pointer to the first element of the linked list of clients 
static pthread_mutex_t clients_mtx = PTHREAD_MUTEX_INITIALIZER; // Mutex to protect client list
extern GROUP * groups; // Pointer to group list
extern pthread_rwlock_t groups_rwlock; // rw lock to protect group list

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
        if(rcvQueryKVSLocalServer(((CLIENT *)client)->clientSocket, &msgId, &buffer1, &buffer2, &buffer2Len) != RCV_QUERY_SUCCESS){
            fprintf(stderr,"Uncommanded disconnection of PID: %d\n",((CLIENT *)client)->PID);
            ((CLIENT *)client)->connectivityStatus = CONN_STATUS_DISCONNECTED;
            // Delete callbacks of client
            callbackDeleteClient(((CLIENT *)client)->cb_sock);
            // Close sockets
            close(((CLIENT *)client)->clientSocket);
            if(((CLIENT *)client)->cb_sock != DISCONNECTED_SOCKET){
                close(((CLIENT *)client)->cb_sock); // Close callback socket
                callbackDeleteClient(((CLIENT *)client)->cb_sock); // Delete callbacks
            }
            pthread_exit(NULL); // Close KVSServerThread
        }
        // ---------- Authenticate client ----------
        // - Establish connection
        if(msgId >= MSG_ID_ESTBL_CONN){ // PIDS greater than MSG_ID_ESTBL_CONN
            // Define client PID
            ((CLIENT *)client)->PID = msgId;
            ((CLIENT *)client)->cb_sock = DISCONNECTED_SOCKET;
            // Output status to msgId just to avoid allocating another variable
            msgId = clientAuth(((CLIENT *)client), buffer1, buffer2);                   
            // If authentication is successful establish connection with callback socket
            if(msgId == STATUS_OK){
                // If connection with callback is unsuccessful it does not report the error 
                // Only if the user attempts to register a callback the error is reported 
                callbackConnect((CLIENT *)client);
            }
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
            case MSG_ID_PUT_VAL:{
                char * cpkey = NULL;
                if(buffer1 != NULL){
                    // Copy key to croadcast callback if out value is valid
                    cpkey = (char*) malloc(strlen(buffer1)+1);
                    strcpy(cpkey,buffer1);
                }
                // Output status to msgId just to avoid allocating another variable
                // Cannot send group poiter as argument beacause it may loose validy 
                // Group acess as to be check inside read lock entries
                //printf("Put value | key: %s | value %s\n",buffer1,buffer2);
                // buffer2Len has length of second element
                msgId = groupAddEntry((CLIENT *) client,buffer1,buffer2);
                ansQueryKVSLocalServer(((CLIENT *)client)->clientSocket,msgId,NULL,0);
                // Memory on buffer1 and buffer2 is not freed because it is used as the 
                // memory allocated for the key value pair
                // On error buffer1 and buffer2 are freed inside groupAddEntry
                // Callbacks is success
                if (msgId == STATUS_OK){
                    pthread_mutex_lock(&((CLIENT *) client)->authGroup_mtx);
                    char * cp_group_id = (char *) malloc(strlen(((CLIENT *) client)->authGroup->id)+1);
                    strcpy(cp_group_id,((CLIENT *) client)->authGroup->id);
                    pthread_mutex_unlock(&((CLIENT *) client)->authGroup_mtx);
                    callbackFlag(cpkey,cp_group_id);
                    free(cp_group_id);
                }
                free(cpkey);
                break;
            }
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
                // Group acess has to be check inside read lock entries
                free(buffer2);
                msgId = groupDeleteEntry((CLIENT *) client,buffer1);
                ansQueryKVSLocalServer(((CLIENT *)client)->clientSocket,msgId,NULL,0);
                // Callbacks 
                if (msgId == STATUS_OK){
                    pthread_mutex_lock(&((CLIENT *) client)->authGroup_mtx);
                    char * cp_group_id = (char *) malloc(strlen(((CLIENT *) client)->authGroup->id)+1);
                    strcpy(cp_group_id,((CLIENT *) client)->authGroup->id);
                    pthread_mutex_unlock(&((CLIENT *) client)->authGroup_mtx);
                    callbackFlag(buffer1,cp_group_id);
                    callbackDeleteKey(buffer1,cp_group_id);
                    free(cp_group_id);
                }
                // Free memory
                free(buffer1);
                break;
        
                break;
            case MSG_ID_REG_CB:
                // Output status to msgId just to avoid allocating another variable
                // Cannot send group poiter as argument beacause it may loose validy 
                // Group acess has to be check inside read lock entries
                msgId = callbackRegister((CLIENT*) client,buffer1,buffer2);
                ansQueryKVSLocalServer(((CLIENT *)client)->clientSocket,msgId,NULL,0);
                free(buffer2);
                // Memory on buffer1 is not freed because it is used as the memory allocated for the key value pair
                // On error buffer1 is freed inside callback
                break;
            // ---------- Close client connection----------
            case MSG_ID_CLOSE_CONN:
                // Commanded disconnection
                msgId = clientDisconnect(client);
                ansQueryKVSLocalServer(((CLIENT *)client)->clientSocket,msgId,NULL,0);
                close(((CLIENT *)client)->clientSocket); // Close socket
                close(((CLIENT *)client)->cb_sock); // Close callback socket
                // If success clear callbacks
                if (msgId == STATUS_OK){
                    callbackDeleteClient(((CLIENT *)client)->cb_sock);
                }
                ((CLIENT *)client)->cb_sock = DISCONNECTED_SOCKET;
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
    // Init access to authenticated group and respective mutex
    newClient->authGroup = NULL;
    pthread_mutex_init(&newClient->authGroup_mtx,NULL); 
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
    pthread_rwlock_rdlock(&groups_rwlock);
    // Allocate pointer to group list
    GROUP * searchPointer = groups;
    // Find group
    while(1){
        // If end of the list is reached without finding the group
        if(searchPointer == NULL){
            pthread_rwlock_unlock(&groups_rwlock);
            // [READ UNLOCK groups]
            return STATUS_GROUP_DSN_EXIST;
        }
        // Check until group is found
        if(strcmp(searchPointer->id,groupId) == 0){
            // The group found cannot be deleted while mutex is locked so it does not become invalid
            // client cannot acess the pointer while mutex is locked so there it does not access something without comparing the secret 
            // There is no loss of efficency because mutex auth group only block delete of this specific group, 
            // and gets and puts of this client, which had to wait either away
            // It allows to lock groups for much less time
             // [LOCk Auth clinet]
            pthread_mutex_lock(&client->authGroup_mtx);
            client->authGroup = searchPointer;
            break;
        }
        // Check next element on the list
        searchPointer = searchPointer->prox;
    }
    pthread_rwlock_unlock(&groups_rwlock);
    // [READ UNLOCK groups]
    // ----------- Authenticate secret ----------
    // If group exists check if secret matches
    char * secretAuth = (char *) malloc(MAX_SECRET_LEN);
    if(secretAuth == NULL){
        client->authGroup = NULL;
        pthread_mutex_unlock(&client->authGroup_mtx);
        return  STATUS_ALLOC_ERROR;
    }else{
        int status = authGetSecret(groupId,secretAuth);
        if(status == AUTH_GROUP_DSN_EXIST){
            client->authGroup = NULL;
            pthread_mutex_unlock(&client->authGroup_mtx);
            free(secretAuth);
            return STATUS_GROUP_DSN_EXIST;
        }else if(status == AUTH_IMPOSSIBLE_SERVER || status == AUTH_SENDING || status == AUTH_RECEIVING || status == AUTH_INVALID){
            client->authGroup = NULL;
            pthread_mutex_unlock(&client->authGroup_mtx);
            free(secretAuth);
            return STATUS_AUTH_COM;
        }else if(status == AUTH_OK ){
            // Compare secret
            if(strcmp(secretAuth,secret)!= 0){
                client->authGroup = NULL;
                pthread_mutex_unlock(&client->authGroup_mtx);
                free(secretAuth);
                return STATUS_ACCSS_DENIED;
            }
        }
    }
    pthread_mutex_unlock(&client->authGroup_mtx);
    // [UNLOCk Auth clinet]
    free(secretAuth);
    // Thsi variable does not really need any synch (the connection)
    client->connectivityStatus = CONN_STATUS_CONNECTED;
    // ----------- Authenticate secret ----------
    client->authGroup = searchPointer;
    return STATUS_OK;
}

int clientDisconnect(CLIENT * client){
    // Check if it is already disconnected
    if (client->connectivityStatus == CONN_STATUS_DISCONNECTED){
        return STATUS_ACCSS_DENIED;
    }
    // Set connectivity status
    client->connectivityStatus = CONN_STATUS_DISCONNECTED;
    // Define disconnection time
    if(clock_gettime(CLOCK_REALTIME, &(client->connTime)) == -1 ) {
        perror("Clock gettime error");
        // Time is not critical so exit is overkill
    }
    // Here no protection to authGroup is needed because it wont be accessed anymore
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
        if(searchPointerPrev->cb_sock != DISCONNECTED_SOCKET){
            close(searchPointerPrev->cb_sock); // Close callback socket
            callbackDeleteClient(searchPointerPrev->cb_sock); // Delete callbacks
        }
        pthread_join(searchPointerPrev->clientThread, NULL); // Wait for client thread
        pthread_mutex_destroy(&searchPointerPrev->authGroup_mtx); // Destroy mutex 
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
            pthread_mutex_lock(&searchPointer->authGroup_mtx);
            searchPointer->authGroup = NULL;
            pthread_mutex_unlock(&searchPointer->authGroup_mtx);
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