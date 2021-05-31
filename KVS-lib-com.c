#include "KVS-lib-base.h" // Include base header
#include "KVS-lib-com.h"

// ---------- Global variables ----------
int clientSock = DISCONNECTED_SOCKET; // client socket
int cb_sock[2]; // Socket for the callback server

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
    #ifdef DEBUG_COM
    printf("Sent msg id: %d.\n",msgId);
    #endif
    int strLen;
    // str1 = NULL if a second argument is not sent
    if(str1 != NULL){
        // 2. Write size of first string
        strLen = strlen(str1)+1; // add+1 to include \0
        if(write(clientSock,&strLen,sizeof(int))<=0){
            return QUERY_COM_ERROR;
        }
        #ifdef DEBUG_COM
        printf("Sent len1: %d.\n",strLen);
        #endif
        // 3. Write first string
        if(write(clientSock,str1,strLen)<=0){
            return QUERY_COM_ERROR;
        }
        #ifdef DEBUG_COM
        printf("Sent str1: %s.\n",str1);
        #endif
    }else{
        strLen = 0;
        if(write(clientSock,&strLen,sizeof(int))<=0){
            return QUERY_COM_ERROR;
        }
        #ifdef DEBUG_COM
        printf("Sent len1: %d.\n",strLen);
        #endif
    }
    // str2 = NULL if a second argument is not sent
    if(str2 != NULL){
        // 4. Write size of second string
        if(write(clientSock,&len2,sizeof(uint64_t))<=0){
            return QUERY_COM_ERROR;
        }
        #ifdef DEBUG_COM
        printf("Sent len2: %llu.\n",len2);
        #endif
        // 5. Write second string
        if(write(clientSock,str2,len2)<=0){
            return QUERY_COM_ERROR;
        }
        #ifdef DEBUG_COM
        printf("Sent str2: %s.\n",str2);
        #endif
    }else{
        len2 = 0;
        if(write(clientSock,&len2,sizeof(uint64_t))<=0){
            return QUERY_COM_ERROR;
        }
        #ifdef DEBUG_COM
        printf("Sent len2: %llu.\n",len2);
        #endif
    }
    // 6. Read status message
    int status;
    if(read(clientSock,&status,sizeof(int))<= 0){
        return QUERY_COM_ERROR;
    }
    #ifdef DEBUG_COM
    printf("Received status %d.\n",status);
    #endif

    // 7. If response contains string response read it,
    uint64_t bytesToRead,nbytes; 
    if(status > 0){
        // Read size of response 
        if(read(clientSock,len3,sizeof(uint64_t))<= 0){
            return QUERY_COM_ERROR;
        }
        #ifdef DEBUG_COM
        printf("Reveived len3: %llu.\n",*len3);
        #endif
        *str3 = (char * ) malloc(*len3);
        bytesToRead = *len3;
        while(bytesToRead > 0){
            nbytes = read(clientSock,*str3+*len3-bytesToRead,bytesToRead);
            if(nbytes <= 0){
                return QUERY_COM_ERROR;
            }else{
                bytesToRead -= nbytes;
            }
        }
        #ifdef DEBUG_COM
        printf("Reveived str3: %s.\n",*str3);
        #endif
    }
    if(msgId == MSG_ID_CLOSE_CONN){
        close(clientSock);
        clientSock = DISCONNECTED_SOCKET;
    }
    // [OUT MUTEX com region]
    switch(status){
    case STATUS_OK:
        return QUERY_OK;
    case STATUS_OK_W_ARG:
        return QUERY_OK;
    case STATUS_ACCSS_DENIED:
        return QUERY_ACCSS_DENIED;
    case STATUS_GROUP_DSN_EXIST:
        return QUERY_GROUP_DSN_EXIST;
    case STATUS_ALLOC_ERROR:
        return QUERY_ALLOC_ERROR;
    default:
        return QUERY_COM_ERROR;
    }
}

