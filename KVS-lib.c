#include "KVS-lib.h" // include header
#include "KVS-lib-com.h"
#include "KVS-lib-MACROS.h"

// ---------- Global variables ----------
extern int clientSock; // client socket
extern struct sockaddr_un server_sock_addr; // server socket address

int establish_connection (char * group_id, char * secret){
    // Create client socket    
    clientSock = socket(AF_UNIX, SOCK_STREAM,0);
    // Ignore socket disconnection signal from client, which is handled when read/write returns -1
    signal(SIGPIPE, SIG_IGN); 
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
    // Query KVS Local server for authentication
    //(int msgId, char * str1, char * str2, uint64_t len2, char * str3, uint64_t * len3)
    switch (queryKVSLocalServer(getpid(),group_id, secret, strlen(secret)+1, NULL,NULL)){
        case QUERY_OK:
            return SUCCESS; 
        case QUERY_ERROR_DISCONNECTED_SOCK:
            return ERROR_DISCONNECTED_SOCK;
        case QUERY_ACCSS_DENIED:
            return ERROR_ACCSS_DENIED;
        case QUERY_GROUP_DSN_EXIST:
            return ERROR_GROUP_DSNT_EXIST;
        case QUERY_ALLOC_ERROR:
            return ERROR_ALLOC;
        case QUERY_COM_ERROR:
            return ERROR_COM_SERVER;
        default:
            return ERROR_COM_SERVER;
    }
}

int put_value(char * key, char * value){
    // Send disconnection request
    int status = queryKVSLocalServer(MSG_ID_PUT_VAL, key, value,strlen(value)+1, NULL,NULL);
    switch(status){
        case QUERY_OK:
            return SUCCESS; 
        case QUERY_ERROR_DISCONNECTED_SOCK:
            return ERROR_DISCONNECTED_SOCK;
        case QUERY_ACCSS_DENIED:
            return ERROR_ACCSS_DENIED;
        case QUERY_GROUP_DSN_EXIST:
            return ERROR_GROUP_DSNT_EXIST;
        case QUERY_ALLOC_ERROR:
            return ERROR_ALLOC;
        case QUERY_COM_ERROR:
            return ERROR_COM_SERVER;
        default:
            return ERROR_COM_SERVER;
    }
}

int close_connection(){
    // Send disconnection request
    int status = queryKVSLocalServer(MSG_ID_CLOSE_CONN, NULL, NULL,0, NULL,NULL);
    // Even if communication with the server is not possible connection is closed
    // The only way an error arises is if the server is either disconnected or 
    // unable to answer the query
    clientSock = DISCONNECTED_SOCKET;
    switch(status){
        case QUERY_OK:
            return SUCCESS; 
        case QUERY_ERROR_DISCONNECTED_SOCK:
            return ERROR_DISCONNECTED_SOCK;
        default:
            return ERROR_COM_SERVER;
    }
}