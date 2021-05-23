#ifndef KVS_LIB_COM_H
#define KVS_LIB_COM_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // to open, close, read
#include <sys/socket.h> // to use sockets
#include <sys/un.h> // to use unix domain sockets
#include <string.h> // to use strcpy
#include <stdint.h> // to use uint16_t

#include "KVS-lib-MACROS.h"

#define QUERY_ERROR_DISCONNECTED_SOCK -1
#define QUERY_ALLOC_ERROR -2
#define QUERY_COM_ERROR -100
int queryKVSLocalServer(int msgId, char * str1, char * str2, uint64_t len2, char ** str3, uint64_t * len3);

#endif