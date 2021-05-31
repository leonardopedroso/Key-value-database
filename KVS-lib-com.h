#ifndef KVS_LIB_COM_H
#define KVS_LIB_COM_H

//#define DEBUG_COM

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // to open, close, read
#include <sys/socket.h> // to use sockets
#include <sys/un.h> // to use unix domain sockets
#include <string.h> // to use strcpy
#include <stdint.h> // to use uint16_t

#include "KVS-lib-MACROS.h"

#define QUERY_OK 0
#define QUERY_ERROR_DISCONNECTED_SOCK -1
#define QUERY_ACCSS_DENIED -2
#define QUERY_GROUP_DSN_EXIST -3
#define QUERY_ALLOC_ERROR -4
#define QUERY_COM_ERROR -100
int queryKVSLocalServer(int msgId, char * str1, char * str2, uint64_t len2, char ** str3, uint64_t * len3);

#define RCV_CALLBACK_OK 0
#define RCV_CALLBACK_ERROR -1
int rcvCallback(int * cb_id, char ** value, uint64_t * valLen);

#endif