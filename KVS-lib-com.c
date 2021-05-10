#include "KVS-lib-com.h"

// ---------- Global variables ----------
int clientSock; // client socket
struct sockaddr_un server_sock_addr; // server socket address

int queryKVSLocalServer(int msgId, char * str1, char * str2, char * str3){
    // [IN MUTEX com region]
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
        strLen = strlen(str2)+1; // add+1 to include \0
        if(write(clientSock,&strLen,sizeof(int))<=0){
            return QUERY_COM_ERROR;
        }
        // 5. Write second string
        if(write(clientSock,str2,strLen)<=0){
            return QUERY_COM_ERROR;
        }
    }else{
        strLen = 0;
        if(write(clientSock,&strLen,sizeof(int))<=0){
            return QUERY_COM_ERROR;
        }
    }
    // 6. Read size of response / status message
    int status;
    if(read(clientSock,&status,sizeof(int))<= 0){
        return QUERY_COM_ERROR;
    }
    // 7. If response contains string response read it,
    int bytesToRead,nbytes; 
    if(status > 0){
        bytesToRead = status;
        while(bytesToRead > 0){
            nbytes = read(clientSock,str3+status-bytesToRead,bytesToRead);
            if(nbytes <= 0){
                return QUERY_COM_ERROR;
            }else{
                bytesToRead -= nbytes;
            }
        }
    }
    // [OUT MUTEX com region]
    return status;
}