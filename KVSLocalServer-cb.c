#include "KVSLocalServer-cb.h"

// ---------- Global variables ----------
CALLBACK * callbacks;

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
