#include "KVSLocalServer-com.h"


int rcvQueryKVSLocalServer(int clientSock, int * msgId, char * str1, char * str2){
    // 1. Receive msg identification corresponding to authentication
    // (no need to check msgId)
    if(read(clientSock,msgId,sizeof(int))<= 0){
        return RCV_QUERY_COM_ERROR;
    }
    // 2. Receive length of first argument 
    int strLen;
    if(read(clientSock,&strLen,sizeof(int))<= 0){
        return RCV_QUERY_COM_ERROR;
    }
    // 3. Read first argument 
    int bytesToRead,nbytes; 
    bytesToRead = strLen;
    while(bytesToRead > 0){
        nbytes = read(clientSock,str1+strLen-bytesToRead,bytesToRead);
        if(nbytes <= 0){
            return RCV_QUERY_COM_ERROR;
        }else{
            bytesToRead -= nbytes;
        }
    }
    // 3. Receive length of second argument 
    if(read(clientSock,&strLen,sizeof(int))<= 0){
        return RCV_QUERY_COM_ERROR;
    }
    // 4. Read first argument if it was sent
    if(strLen != 0){
        int bytesToRead,nbytes; 
        bytesToRead = strLen;
        while(bytesToRead > 0){
            nbytes = read(clientSock,str2+strLen-bytesToRead,bytesToRead);
            if(nbytes <= 0){
                return RCV_QUERY_COM_ERROR;
            }else{
                bytesToRead -= nbytes;
            }
        }
    }
    return RCV_QUERY_SUCCESS;
}

int ansQueryKVSLocalServer(int clientSock, int status, char * str1){
    // Send status / response string length
    if(write(clientSock,&status,sizeof(int))<= 0){
        return ANS_QUERY_COM_ERROR;
    }
    if(status >= 0){
        // Send string response
        if(write(clientSock,str1,status)<= 0){
            return ANS_QUERY_COM_ERROR;
        }
    }
    return ANS_QUERY_SUCCESS;
}