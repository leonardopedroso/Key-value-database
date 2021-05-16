#include "KVSLocalServer.h" // Header

// ---------- Global variables ----------
int flagShutDown = RUNNING; // set to one if receives shutdown command 
 

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
    printf("Welcome to KVS Local Server console.\n");
    // Print menu once
    printMenu();
    // Wait for commands in console user interface
    while(1){
        printf(">> "); // print console indicator
        switch(getCommand(&group[0])){
            case CREATE_DES: // Create group
                // Catch group create error
                switch(groupAdd(&group[0])){
                    case GROUP_OK:
                        break;
                    case GROUP_ALLOC_ERROR:
                        fprintf(stderr,"Unable to create group: Allocation error has occurred.\n");
                        break;
                    case GROUP_ALREADY_EXISTS:
                        fprintf(stderr,"Unable to create group: Group already exists.\n");
                        break;
                    case GROUP_AUTH_COM_ERROR:
                        fprintf(stderr,"Unable to create group: Communication with authentication server failed.\n");
                        break;
                    case GROUP_LOSS_SYNCH:
                        fprintf(stderr,"Unable to create group: Lost synchronization with authentication server.\n");
                    default:
                        fprintf(stderr,"Unable to create group: Unknown exception.\n");
                        break;
                }
                break;
            case DELETE_DES: // Delete group
                // Catch group delete error
                switch(groupDelete(&group[0])){
                    case GROUP_OK:
                        break;
                    case GROUP_DSNT_EXIST:
                        fprintf(stderr,"Unable to delete group: Group does not exist.\n");
                        break;
                    default:
                        fprintf(stderr,"Unable to delete group: Unknown exception.\n");
                        break;
                }
                break;
            case GROUP_DES: // Show group
                switch(groupShow(&group[0])){
                    case GROUP_OK:
                        break;
                    case GROUP_ALLOC_ERROR:
                        fprintf(stderr,"Unable to show group: Allocation error has occurred.\n");
                        break;
                    case GROUP_DSNT_EXIST:
                        fprintf(stderr,"Unable to show group: Group does not exist.\n");
                        break;
                    case GROUP_LOSS_SYNCH:
                        fprintf(stderr,"Unable to show group: Lost synchronization with authentication server.\n");
                        break;
                    case GROUP_AUTH_COM_ERROR:
                        fprintf(stderr,"Unable to show group: Communication with authentication server failed.\n");
                        break;
                    default:
                        fprintf(stderr,"Unable to show group: Unknown exception.\n");
                        break;
                }
                break;
            case APPS_DES:
                clientShow();
                break;
            case EXIT_DES:
                flagShutDown = 1; // Set shutdown flag
                break;
            default: // Invalid command
                printMenu(); // Print menu again
                break;
        }
        if(flagShutDown)break; // handle shut down
    }
    // ---------- Shutdown procedure ----------
    close(server_sock); // Close socket listening for connections
    remove(KVS_LOCAL_SERVER_ADDR); // Remove address
    pthread_join(serverThread,NULL); // Wait for server thread to quit
    switch(flagShutDown){
        case SD_CONTROLLED:
            printf("KVS local server shutdown completed.\n");
            break;
        case SD_ACCEPT_ERROR:
            fprintf(stderr,"KVS local server was forced to shutdown: Unable to accept connections.\n");
            break;
        default:
            fprintf(stderr,"KVS local server was forced to shutdown: Unknown exception.\n");
            break;
    }
    closeClients(); // Close connections to the clients, join repective threads, and free memory
    groupClear(); // Clear memory of all groups 
    exit(0);
    // [PROBLEMA DOS DIABOS]
    // [SE SE TIVER DE FAZER ABORT FORA DA MAIN POR EXEMPLO EM KVS LOCAL SERVER THREAD, FICAMOS PRESOS NO PRINTF]
    // MANDAR SINAL PARA DESBLOQUER O PRINTF
}

// ---------- KVS Server thread function ----------

void * KVSLocalServerThread(void * server_sock){
    // Variable to hold new socket when a new client connects
    int clientSocket;
    // Wait for connections and handle them 
    while(1){
        // Wait for a connection (the server socket is passed as an argument to KVSLocalServerThread)
        clientSocket = accept(*((int *) server_sock), NULL, NULL);
        // Catch error waiting for a connection
        if (clientSocket == -1){
            break;
        }
        // Add client and handle it in a new thread
        // Catch errors handling new client
        if(clientHandle(clientSocket) == ERROR_CLIENT_ALLOCATION){
            flagShutDown = SD_ALLOCATION_ERROR;
            break;
        }
        printf("Accepted new connection.\n");
    }
    switch(flagShutDown){
        case RUNNING:
            flagShutDown = SD_ACCEPT_ERROR;
            fprintf(stderr,"Main server thread aborted: Unable to accept connections.\n");
            break;
        case SD_CONTROLLED:
            printf("Main server thread was successfully shutdown.\n");
            break;
        case SD_ALLOCATION_ERROR:
            fprintf(stderr,"Main server thread aborted: Allocation error.\n");
            break;
        default:
            fprintf(stderr,"Main server thread aborted: Unknown exception.\n");
            break;
    }
    pthread_exit(NULL); // Close KVSServerThread
}