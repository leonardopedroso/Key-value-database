#ifndef KVS_LOCAL_SERVER_COM_H
#define KVS_LOCAL_SERVER_COM_H

#include "KVSLocalServer-base.h"

#define DEBUG_COM

#define RCV_QUERY_SUCCESS 0
#define RCV_QUERY_ALLOC_ERROR -1
#define RCV_QUERY_COM_ERROR -100
int rcvQueryKVSLocalServer(int clientSock, int * msgId, char ** str1, char ** str2, uint64_t * len2);

#define ANS_QUERY_SUCCESS 0
#define ANS_QUERY_COM_ERROR -100
int ansQueryKVSLocalServer(int clientSock, int status, char * str1, uint64_t len);



#endif