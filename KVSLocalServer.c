#include "KVSLocalServer.h" // Header
#include "KVSLocalServer-com.h" // Local communication functions 
#include "KVS-lib-MACROS.h" // Shared MACROS
#include "ui.h" // User interface


// ---------- Global variables ----------
CLIENT * clients = NULL; // Pointer to the first element of the linked list of clients 
GROUP * groups = NULL; // Pointer to the first element of the linked list of groups 

int main(){
    
    // ---------- Setup server variables ----------
    int server_sock; // fd of rcv socket
    struct sockaddr_un server_sock_addr; // struct addr of sever socket
    server_sock_addr.sun_family = AF_UNIX; // set socket family type
    
    // ---------- Setup server socket ----------
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0); // create server socket
    // Ignore socket disconnection signal from client, which is handled when read returns -1
    signal(SIGPIPE, SIG_IGN); 
    // Catch error creating reception socket
    if (server_sock == -1){
        perror("Error creating reception socket");
        exit(-1);
    }
    printf("Reception socket created\n");
    
    // ---------- Bind server socket ----------
    // unlink server if last server session was not terminated properly
    unlink(KVS_LOCAL_SERVER_ADDR); 
    strcpy(server_sock_addr.sun_path, KVS_LOCAL_SERVER_ADDR); // set socket to known address
    // Catch error binding socket to address
    if( bind(server_sock, (struct sockaddr *) &server_sock_addr, sizeof(struct sockaddr_un)) == -1){
        perror("Error binding address to socket");
        exit(-1);
    }
    printf("Reception socket binded to address %s\n", server_sock_addr.sun_path);
    
    // ---------- Listen to incoming connections ----------
    // Catch error listening to connections
    if( listen(server_sock,KVS_LOCAL_SERVER_BACKLOG) == -1){
        perror("Error listening to incoming connections");
        exit(-1);
    }
    printf("Reception socket is listening to connections.\n");

    // ---------- Wait and handle connections on thread ----------
    pthread_t serverThread;
    pthread_create(&serverThread, NULL, &KVSLocalServerThread, &server_sock);

    // ---------- Server console ----------
    // Allocate buffer 
    // Maximum size is used just here, so that it not necessary to do mallocs inside ui files
    // The corresponding frees would have to be done outside the allocating files 
    // So we chose clear code over messy code with slightly less memory needs
    char group[MAX_STR_LENGTH];
    // Print menu once
    printMenu();
    // Wait for commands in console user interface
    while(1){
        switch(getCommand(&group[0])){
            case CREATE_DES:
                printf("Created group %s with secret __\n",group);
                groupAdd(&group[0]);
                // [COMMUNICATE WITH AUTH SERVER]
                // generateSecret();
                // BrodcastSecret();
                break;
            case DELETE_DES:
                switch(groupDelete(&group[0])){
                    case 1:
                        break;
                }
                printf("Deleted group %s\n\n",group);
                break;
            case GROUP_DES:
                printf("Showing group %s\n\n",group);
                break;
            case APPS_DES:
                printf("Showing apps\n\n");
                break;
            default:
                printf("\n");
                printMenu();
                break;
        }
    }
    exit(0);
}

// ---------- KVS Server thread functions ----------

void * KVSLocalServerThread(void * server_sock){
    // Variable to hold new socket when a new client connects
    int clientSocket;
    // Wait for connections and handle them 
    while(1){
        // Wait for a connection (the server socket is passed as an argument to KVSLocalServerThread)
        clientSocket = accept(*((int *) server_sock), NULL, NULL);
        // Catch error waiting for a connection
        if (clientSocket == -1){
            // [TO DO] DISTINGUISH BETWEEN ERRORS OR JUST SERVER CLOSE
            printf("Stopped main server thread.\n");
            break;
        }
        // Add client and handle it in a new thread
        // Catch errors handling new client
        if(clientHandle(clientSocket) <0 ){
            printf("Error handling new client.\n");
            break;
        }
        printf("Accepted new connection.\n");
    }
    closeClients(); // Close connections to the clients, join repective threads, and free memory
    pthread_exit(NULL); // Close KVSServerThread
}

void * KVSLocalServerClientThread(void * client){
    // Allocate buffers
    char buffer1[MAX_STR_LENGTH];
    int buffer1Len;
    char buffer2[MAX_STR_LENGTH];
    int buffer2Len;
    int msgId = 0;
    
    // Loop receiving and handling queries
    while(1){
        if(rcvQueryKVSLocalServer(((CLIENT *)client)->clientSocket, &msgId, &buffer1[0], &buffer2[0]) == RCV_QUERY_COM_ERROR){
            // [CUIDADO QUANDO TIVER O CALLBACK]
            printf("Uncommanded disconnection of PID: %d\n",((CLIENT *)client)->PID);
            close(((CLIENT *)client)->clientSocket);
            pthread_exit(NULL); // Close KVSServerThread
        }
        // ---------- Authenticate client ----------
        if(msgId >= MSG_ID_ESTBL_CONN){
            // Define client PID
            ((CLIENT *)client)->PID = msgId;
            // Output to msgId just to avoid allocating another variable
            msgId = clientAuth(((CLIENT *)client));
            ansQueryKVSLocalServer(((CLIENT *)client)->clientSocket,msgId,NULL);
            printf("Client authenticated | Group: %s | Secret: %s | PID: %d\n", buffer1,buffer2,((CLIENT *)client)->PID);
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
                ansQueryKVSLocalServer(((CLIENT *)client)->clientSocket,msgId,NULL);
                // [CUIDADO QUANDO TIVER O CALLBACK]
                close(((CLIENT *)client)->clientSocket);
                pthread_exit(NULL); // Close KVSServerThread
                break;
            default:
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
        perror("Error alocating memory to new client");
        return ERROR_CLIENT_ALLOCATION;
    }
    // Store socket for communication with this client
    newClient->clientSocket = clientSocket;
    // Define connectivity status of client
    newClient->connectivityStatus = CONN_STATUS_NOT_AUTH;
    // Define connection time
    if(clock_gettime(CLOCK_REALTIME, &(newClient->connTime)) == -1 ) {
        perror("Clock gettime error");
        // Time is not critical so exit is overkill
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

int clientAuth(CLIENT * client){
    // [CHECK AUTHENTICATION AUTH SERVER]
    // switch(authenticated)
    // case OK:
        client->connectivityStatus = CONN_STATUS_CONNECTED;
        // return query status
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
    printf("Commanded disconnection of PID: %d\n",client->PID);
    return STATUS_OK;
}

// REDO BELOW
void closeClients(){
    // ---------- Remove client block to the linked list ----------
    // [CUIDADO NO FUTURO COM POSSIVEIS PROBLEMAS DE SINCRONIZAÇÃO]
    CLIENT * searchPointer = clients;
    CLIENT * searchPointerPrev;
    // Check if pointer to the linked list is NULL (i.e. there are no connected clients)
    if(searchPointer == NULL){
       return;
    }
    // Iterate through the clients closing, joining, and freeing memory
    while(searchPointer != NULL){
        searchPointerPrev = searchPointer;
        searchPointer = searchPointer->prox;
        // Close socket 
        close(searchPointerPrev->clientSocket);
        // Wait for client thread
        pthread_join(searchPointerPrev->clientThread, NULL);
        // Free memory allocated for client
        free(searchPointerPrev);
    }
}

