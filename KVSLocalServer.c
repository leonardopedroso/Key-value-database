#include "KVSLocalServer.h" // Header
 
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
                switch(groupAdd(&group[0])){
                    case GROUP_OK:
                        printf("Created group %s with secret __\n",group);
                        break;
                }
                // [COMMUNICATE WITH AUTH SERVER]
                // generateSecret();
                // brodcastSecret();
                break;
            case DELETE_DES:
                switch(groupDelete(&group[0])){
                    case GROUP_OK:
                        printf("Deleted group %s\n\n",group);
                        break;
                }
                break;
            case GROUP_DES:
                switch(groupShow(&group[0])){
                    case GROUP_OK:
                        printf("Showing group %s\n\n",group);
                        break;
                }
                break;
            case APPS_DES:
                clientShow();
                break;
            default: // Invalid command
                printf("\n");
                printMenu(); // Print menu again
                break;
        }
    }
    exit(0);
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
    //[Handle client disconnections]
    //closeClients(); // Close connections to the clients, join repective threads, and free memory
    pthread_exit(NULL); // Close KVSServerThread
}