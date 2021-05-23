#include "KVS-lib-com.h"

// ---------- Global variables ----------
int clientSock = DISCONNECTED_SOCKET; // client socket
struct sockaddr_un server_sock_addr; // server socket address

int queryKVSLocalServer(int msgId, char * str1, char * str2, uint64_t len2, char ** str3, uint64_t * len3){
    // [IN MUTEX com region]
    // Check if socket has been connected
    if(clientSock == DISCONNECTED_SOCKET){
        return QUERY_ERROR_DISCONNECTED_SOCK;
    }
    // Protocol for communication with server:
    // 1. Write message identification
    if(write(clientSock,&msgId,sizeof(int))<=0){
        return QUERY_COM_ERROR;
    }
    int strLen;
    // str1 = NULL if a second argument is not sent
    if(str1 != NULL){
        // 2. Write size of first string
        strLen = strlen(str1)+1; // add+1 to include \0
        if(write(clientSock,&strLen,sizeof(int))<=0){
            return QUERY_COM_ERROR;
        }
        // 3. Write first string
        if(write(clientSock,str1,strLen)<=0){
            return QUERY_COM_ERROR;
        }
    }else{
        strLen = 0;
        if(write(clientSock,&strLen,sizeof(int))<=0){
            return QUERY_COM_ERROR;
        }
    }
    // str2 = NULL if a second argument is not sent
    if(str2 != NULL){
        // 4. Write size of second string
        if(write(clientSock,&len2,sizeof(uint64_t))<=0){
            return QUERY_COM_ERROR;
        }
        // 5. Write second string
        if(write(clientSock,str2,len2)<=0){
            return QUERY_COM_ERROR;
        }
    }else{
        len2 = 0;
        if(write(clientSock,&len2,sizeof(uint64_t))<=0){
            return QUERY_COM_ERROR;
        }
    }
    // 6. Read status message
    int status;
    if(read(clientSock,&status,sizeof(int))<= 0){
        return QUERY_COM_ERROR;
    }

    // 7. If response contains string response read it,
    int bytesToRead,nbytes; 
    if(status > 0){
        // Read size of response 
        if(read(clientSock,len3,sizeof(uint64_t))<= 0){
            return QUERY_COM_ERROR;
        }
        *str3 = (char * ) malloc(*len3);
        bytesToRead = status;
        while(bytesToRead > 0){
            nbytes = read(clientSock,*str3+status-bytesToRead,bytesToRead);
            if(nbytes <= 0){
                return QUERY_COM_ERROR;
            }else{
                bytesToRead -= nbytes;
            }
        }
    }
    if(msgId == MSG_ID_CLOSE_CONN){
        close(clientSock);
        clientSock = DISCONNECTED_SOCKET;
    }
    // [OUT MUTEX com region]
    switch(status){
    case STATUS_OK:
        return QUERY_OK;
    case STATUS_ACCSS_DENIED:
        return QUERY_ACCSS_DENIED;
    case STATUS_GROUP_DSN_EXIST:
        return QUERY_GROUP_DSN_EXIST;
    default:
        return QUERY_COM_ERROR;
    }
}