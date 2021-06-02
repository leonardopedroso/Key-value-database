#include "KVS-lib.h" // include header
#include "KVS-lib-base.h" // Include base header
#include "KVS-lib-com.h"
#include "KVS-lib-cb.h"

// ---------- Global variables ----------
// Communication with KVS Local Server
extern int clientSock; // client socket
// Callback
extern int cb_sock[2]; // callback socket
extern pthread_t cbThread; // callback thread
extern CALLBACK * callbacks;

int establish_connection (char * group_id, char * secret){
    // Check socket status
    if (clientSock != DISCONNECTED_SOCKET){
        return ERROR_ALRDY_CONNECTED_SOCK;
    }
    // ---------- Setup server connection ----------
    // Create client socket    
    clientSock = socket(AF_UNIX,SOCK_STREAM,0);
    // Ignore socket disconnection signal from client, which is handled when read/write returns -1
    signal(SIGPIPE, SIG_IGN); 
    // Catch errors creating socket
    if (clientSock == DISCONNECTED_SOCKET){
        perror("Error creating socket");
        return ERROR_CREATING_SOCK;
    }
    // Setup server connection
    struct sockaddr_un server_sock_addr;
    server_sock_addr.sun_family = AF_UNIX;
    strcpy(server_sock_addr.sun_path, KVS_LOCAL_SERVER_ADDR);
    if(connect(clientSock, (struct sockaddr *) &server_sock_addr, sizeof(struct sockaddr_un)) == -1){
        perror("Error connecting to server");
        return ERROR_CONNECTION_SERVER;
    }

    // ---------- Setup callback socket ----------
    // Setup server variables ----------
    struct sockaddr_un cb_sock_addr; // struct addr of sever socket
    cb_sock_addr.sun_family = AF_UNIX; // set socket family type

    // Setup server socket ----------
    cb_sock[0] = socket(AF_UNIX, SOCK_STREAM, 0); // create server socket
    cb_sock[1] = DISCONNECTED_SOCKET; // init value of connection socket
    // Ignore socket disconnection signal from client, which is handled when read/write returns -1
    signal(SIGPIPE, SIG_IGN); 
    // Catch error creating reception socket
    if (cb_sock[0] == DISCONNECTED_SOCKET){
        perror("Error creating callback socket");
        return ERROR_CALLBACK_SOCK;
    }
    #ifdef DEBUG_CALLBACK
    printf("Callback socket created.\n");
    #endif
    char cb_server_addr[MAX_LEN_CN_SERVER_ADDR];
    sprintf(cb_server_addr,"/tmp/cb%d",getpid());

    // Bind server socket ----------
    // unlink server if last server session was not terminated properly
    unlink(cb_server_addr); 
    strcpy(cb_sock_addr.sun_path, cb_server_addr); // set socket to known address
    // Catch error binding socket to address
    if( bind(cb_sock[0], (struct sockaddr *) &cb_sock_addr, sizeof(struct sockaddr_un)) == -1){
        perror("Error binding address to callback socket");
        return ERROR_CALLBACK_SOCK;
    }
    #ifdef DEBUG_CALLBACK
    printf("Callback socket binded to address %s\n",cb_sock_addr.sun_path);
    #endif

    // Listen to incoming connections ----------
    // Catch error listening to connections
    if(listen(cb_sock[0],CB_SERVER_BACKLOG) == -1){
        perror("Error listening to incoming callback connection");
        return ERROR_CALLBACK_SOCK;
    }
    #ifdef DEBUG_CALLBACK
    printf("Callback socket is listening for KVS local server.\n");
    #endif

    // Start calback thread
    pthread_create(&cbThread, NULL, &callbackServerThread, NULL);

    // ---------- Query KVS Local server for authentication
    //(int msgId, char * str1, char * str2, uint64_t len2, char * str3, uint64_t * len3)
    int statusQuery = queryKVSLocalServer(getpid(),group_id, secret, strlen(secret)+1, NULL,NULL);
    // If authentication was unsuccessful shutdown call back thread
    if (statusQuery != QUERY_OK){ 
        callbackDisconnect();
    }
    switch (statusQuery){
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
    // Send put value request
    int status = queryKVSLocalServer(MSG_ID_PUT_VAL,key,value,strlen(value)+1,NULL,NULL);
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

int get_value(char * key, char ** value){
    // Send put value request
    uint64_t len; // unused (we believe it may be useful to output th enumber of bytes read)
    int status = queryKVSLocalServer(MSG_ID_GET_VAL,key,NULL,0,value,&len);
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

int delete_value(char * key){
    // Send put value request
    int status = queryKVSLocalServer(MSG_ID_DEL_VAL,key,NULL,0,NULL,NULL);
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

int register_callback(char * key, void (*callback_function)(char *)){
    // Check if callback server is up and running
    if(cb_sock[1]==DISCONNECTED_SOCKET){
        return ERROR_CALLBACK_COM_ERROR;
    }
    // Add to callback list and find callback id in the aplication
    int cb_id = callbackAdd(key,callback_function);
    if (cb_id < 0){ // Catch error 
        switch (cb_id){
        case CALLBACK_ALLOC_ERROR:
            return ERROR_ALLOC;
        default:
            return ERROR_CALLBACK_COM_ERROR;
        }
    }
    // Send regiter callback request
    // Callback id is sent as a byte stream
    int status = queryKVSLocalServer(MSG_ID_REG_CB,key,(char*)&cb_id,sizeof(int),NULL,NULL);
    switch(status){
        case QUERY_OK:
            return SUCCESS; 
        case QUERY_ERROR_DISCONNECTED_SOCK:
            return ERROR_DISCONNECTED_SOCK;
        case QUERY_ACCSS_DENIED:
            return ERROR_ACCSS_DENIED;
        case QUERY_ALLOC_ERROR:
            return ERROR_ALLOC;
        case QUERY_COM_ERROR:
            return ERROR_COM_SERVER;
        case QUERY_KEY_DSNT_EXIST:
            return ERROR_KEY_DSNT_EXIST;
        default:
            return ERROR_COM_SERVER;
    }
}

int close_connection(){
    // Check status of connection
    if (clientSock == DISCONNECTED_SOCKET){
        return ERROR_DISCONNECTED_SOCK;
    }
    // Send disconnection request
    int status = queryKVSLocalServer(MSG_ID_CLOSE_CONN, NULL, NULL,0, NULL,NULL);
    // Even if communication with the server is not possible connection is closed
    // The only way an error arises is if the server is either disconnected or 
    // unable to answer the query
    clientSock = DISCONNECTED_SOCKET;
    callbackDisconnect();
    switch(status){
        case QUERY_OK:
            return SUCCESS; 
        case QUERY_ERROR_DISCONNECTED_SOCK:
            return ERROR_DISCONNECTED_SOCK;
        default:
            return ERROR_COM_SERVER;
    }
}
