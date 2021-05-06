#include "KVS-lib.h" // include header

// ---------- Global variables ----------
int clientSock; // client socket
struct sockaddr_un server_sock_addr; // server socket address

int establish_connection (char * group_id, char * secret){
    // Create client socket    
    clientSock = socket(AF_UNIX, SOCK_STREAM,0);
    // Catch errors creating socket
    if (clientSock == -1){
        perror("Error creating socket");
        return ERROR_CREATING_SOCK;
    }
    // Setup server connection
    server_sock_addr.sun_family = AF_UNIX;
    strcpy(server_sock_addr.sun_path, KVS_LOCAL_SERVER_ADDR);
    if(connect(clientSock, (struct sockaddr *) &server_sock_addr, sizeof(struct sockaddr_un)) == -1){
        perror("Error connecting to server");
        return ERROR_CONNECTION_SERVER;
    }

    // [FIQUEI AQUI - LEO]
    // AGORA É PRECISO FAZER O HANDSHAKE:
    // 1. MANDAR GRUPO e SEGREDO
    // 2. RECEBER OK OU ACESSO NEGADO

    // ERRORS:
    // i) CREATING SOCKET
    // ii) CONNECTING TO SERVER 
    // ii) ...

    return SUCCESS_ESTABLISH_CONNECTION;
}