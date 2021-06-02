#include "KVSLocalServer.h" // Header
#include "KVSLocalServer-auth.h" // Auth server
#include "KVSLocalServer-data.h" // Data management functions
#include "KVSLocalServer-client.h" // Client management functions
#include "ui.h" // User interface

// ---------- Global variables ----------
int shutdownPipeFd;
 
int main(){
    // ---------- Setup authentication server ----------
    switch(initCom()){
        case SUCCESS_SOCK:
            fprintf(stderr,"Connection with authentication established.\n");
            break;
        case ERR_CREATING_SOCK:
            fprintf(stderr,"Error crreating socket for authentication.\n");
            exit(-1);
        case ERR_CONVERTING_IP:
            fprintf(stderr,"Error converting IP for authentication.\n");
            exit(-1);
    }

    // ---------- Setup server variables ----------
    int server_sock; // fd of rcv socket
    struct sockaddr_un server_sock_addr; // struct addr of sever socket
    server_sock_addr.sun_family = AF_UNIX; // set socket family type
    
    // ---------- Setup server socket ----------
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0); // create server socket
    // Ignore socket disconnection signal from client, which is handled when read/write returns -1
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
    if(listen(server_sock,KVS_LOCAL_SERVER_BACKLOG) == -1){
        perror("Error listening to incoming connections");
        exit(-1);
    }
    printf("Reception socket is listening to connections.\n");

    // ---------- Setup shutdown thread ----------
    int shutdownPipeFds[2];
    if(pipe(shutdownPipeFds) == -1){
        perror("Error creating shutdown pipe");
        exit(-1);
    }
    shutdownPipeFd = shutdownPipeFds[1]; // Set writing fd to glbal variable
    //close(shutdownPipeFds[1]); // Close file descriptor on main
    pthread_t shutdownServer, serverThread; // Allocate shutdown server and server thread
    // Start shutdown server
    pthread_create(&shutdownServer, NULL, &KVSLocalServerShutdownThread, &shutdownPipeFds[0]); 
    // Init shutdown server
    // [TALVEZ POR UMA FLAG E FAZER JOIN DO THREAD SHUTDWON]
    if(write(shutdownPipeFd,&server_sock,sizeof(int))<= 0){
        fprintf(stderr,"Shutdown thread initialization failed: Unable to identify server socket descriptor.\n");
        exit(-1);
    }
    pthread_t * serverThreadPtr;
    serverThreadPtr = &serverThread;
    if(write(shutdownPipeFd,&serverThreadPtr,sizeof(pthread_t *))<= 0){
        fprintf(stderr,"Shutdown thread initialization failed: Unable to identify server thread.\n");
        exit(-1);
    }
    // ---------- Wait and handle connections on thread ----------
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
            case EXIT_DES:{
                // Flag shutdown pipe
                int flag = SD_CONTROLLED;
                if(write(shutdownPipeFd,&flag,sizeof(int))<= 0){
                    printf("KVS local server was signaled to shutdown.\n");
                    fprintf(stderr,"Shutdown pipe error: Unable to flag shutdown.\n");
                    fprintf(stderr,"Uncontrolled shutdown.\n");
                    exit(-1);
                }
                break;
            }
            default: // Invalid command
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
            // Error can arise either by an actual error or by the controlled shutdown
            // Send shutdown flag just in case (if it was a controlled shutdown does nothing) 
            int flag = SD_ACCEPT_ERROR;
            if(write(shutdownPipeFd,&flag,sizeof(int))<= 0){
                fprintf(stderr,"KVS local server was forced to shutdown: Unable to accept connections.\n");
                fprintf(stderr,"Shutdown pipe error: Unable to flag shutdown.\n");
                fprintf(stderr,"Uncontrolled shutdown.\n");
                exit(-1);
            }
            break;
        }
        // Add client and handle it in a new thread
        // Catch errors handling new client
        if(clientHandle(clientSocket) == ERROR_CLIENT_ALLOCATION){
            int flag = SD_ALLOCATION_ERROR;
            if(write(shutdownPipeFd,&flag,sizeof(int))<= 0){
                fprintf(stderr,"KVS local server was forced to shutdown: Allocation error.\n");
                fprintf(stderr,"Shutdown pipe error: Unable to flag shutdown.\n");
                fprintf(stderr,"Uncontrolled shutdown.\n");
                exit(-1);
            }
            break;
        }
        printf("Accepted new connection.\n");
    }
    pthread_exit(NULL); // Close KVSServerThread
}

// ---------- Controlled shutdown thread ----------
void * KVSLocalServerShutdownThread(void * arg){
    // ---------- Init shutdown pipe ----------
    // Get server socket
    int server_sock, flagSD = 0;
    if(read(*((int *)arg),&server_sock,sizeof(int))<=0){
        fprintf(stderr,"Shutdown thread initialization failed: Unable to read server socket descriptor.\n");
        flagSD = SD_SD_PIPE_ERROR;
    }
    // Get server thread
    pthread_t * serverThread;
    if( read(*((int *)arg),&serverThread,sizeof(pthread_t *))<=0){
        fprintf(stderr,"Shutdown thread initialization failed: Unable to identify server thread.\n");
        flagSD = SD_SD_PIPE_ERROR;
    }
    if(flagSD!=0){
        printf("Shutdown thread initialization successful.\n");
    }
    // ---------- Wait for shutdown flag ----------
    if(read(*((int *)arg),&flagSD,sizeof(int))<=0){
        fprintf(stderr,"Shutdown thread aborted: Unable to listen for shutdown queries.\n");
        flagSD = SD_SD_PIPE_ERROR;
    }
    // ---------- Shutdown procedure ----------
    // - Close thread accepting connections
    close(server_sock); // Close socket listening to connections
    remove(KVS_LOCAL_SERVER_ADDR); // Remove address
   
    // - Close clients and free memory allocated to them 
    closeClients(); // Close connections to the clients, join repective threads, and free memory
    // - Clear memory of key value pairs
    groupClear(); // Clear memory of all groups 
    // - Close shutdown pipe
    close(shutdownPipeFd); // Close wiriting to shutdoen pipe
    close(*((int *)arg)); // Reading from shutdown pipe
    switch(flagSD){
        case SD_CONTROLLED:
            printf("KVS local server shutdown completed.\n");
            break;
        case SD_ACCEPT_ERROR:
            fprintf(stderr,"KVS local server was forced to shutdown: Unable to accept connections.\n");
            break;
        case SD_ALLOCATION_ERROR:
            fprintf(stderr,"KVS local server was forced to shutdown: Allocation error.\n");
            break;
        case SD_SD_PIPE_ERROR:
            fprintf(stderr,"KVS local server was forced to shutdown: Shutdown thread error.\n");
            break;
        default:
            fprintf(stderr,"KVS local server was forced to shutdown: Unknown exception.\n");
            break;
    }
    exit(0);
}