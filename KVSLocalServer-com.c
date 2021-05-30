#include "KVSLocalServer-com.h"


int rcvQueryKVSLocalServer(int clientSock, int * msgId, char ** str1, char ** str2, uint64_t * len2){
    // 1. Receive msg identification corresponding to authentication
    if(read(clientSock,msgId,sizeof(int))<= 0){
        return RCV_QUERY_COM_ERROR;
    }
    #ifdef DEBUG_COM
    printf(" == New query received == \n");
    printf("Reveived msg id: %d.\n",*msgId);
    #endif
    // 2. Receive length of first argument 
    int strLen;
    if(read(clientSock,&strLen,sizeof(int))<= 0){
        return RCV_QUERY_COM_ERROR;
    }
    #ifdef DEBUG_COM
    printf("Reveived len1: %d.\n",strLen);
    #endif
    if(strLen != 0){
        // 3. Read first argument 
        *str1 = (char * ) malloc(strLen); //Allocate memory for first string
        // Catch allocation error
        if (*str1 == NULL){
            fprintf(stderr,"Recive query failed: Allocation error.");
            return RCV_QUERY_ALLOC_ERROR;
        }
        int bytesToRead,nbytes; 
        bytesToRead = strLen;
        while(bytesToRead > 0){
            nbytes = read(clientSock,*str1+strLen-bytesToRead,bytesToRead);
            if(nbytes <= 0){
                return RCV_QUERY_COM_ERROR;
            }else{
                bytesToRead -= nbytes;
            }
        }
        #ifdef DEBUG_COM
        printf("Reveived str1: %s.\n",*str1);
        #endif
    }
    // 3. Receive length of second argument 
    if(read(clientSock,len2,sizeof(uint64_t))<= 0){
        return RCV_QUERY_COM_ERROR;
    }
    #ifdef DEBUG_COM
    printf("Reveived len2: %llu.\n",*len2);
    #endif
    // 4. Read second argument if it was sent
    
    if(*len2 != 0){
        *str2 = (char *) malloc(*len2); // Aloocate memory for second string
        // Catch allocation error
        if (*str2 == NULL){
            fprintf(stderr,"Recive query failed: Allocation error.");
            free(*str1); // Free aloocated memory
            return RCV_QUERY_ALLOC_ERROR;
        }
        int bytesToRead,nbytes; 
        bytesToRead = *len2;
        while(bytesToRead > 0){
            nbytes = read(clientSock,*str2+*len2-bytesToRead,bytesToRead);
            if(nbytes <= 0){
                // In the event of an error free located memory
                free(*str1);
                free(*str2);
                return RCV_QUERY_COM_ERROR;
            }else{
                bytesToRead -= nbytes;
            }
        }
        #ifdef DEBUG_COM
        printf("Reveived str2: %s.\n",*str2);
        #endif
    }
    #ifdef DEBUG_COM
    printf(" == Full query received == \n");
    #endif
    return RCV_QUERY_SUCCESS;
}

int ansQueryKVSLocalServer(int clientSock, int status, char * str1, uint64_t len){
    // Check if there is an augument to send and ajust status accordingly
    if(status == STATUS_OK && str1 != NULL){
        status = STATUS_OK_W_ARG;
    }
    // Send status 
    if(write(clientSock,&status,sizeof(int))<= 0){
        return ANS_QUERY_COM_ERROR;
    }
    if(status == STATUS_OK_W_ARG){
        // Send response string length
        if(write(clientSock,&len,sizeof(uint64_t))<= 0){
            return ANS_QUERY_COM_ERROR;
        }
        // Send string response
        if(write(clientSock,str1,len)<= 0){
            return ANS_QUERY_COM_ERROR;
        }
    }
    return ANS_QUERY_SUCCESS;
}