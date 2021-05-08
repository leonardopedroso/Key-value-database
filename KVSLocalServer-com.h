#ifndef KVS_LOCAL_SERVER_COM_H
#define KVS_LOCAL_SERVER_COM_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // to open, close, read
#include <sys/socket.h> // to use sockets
#include <sys/un.h> // to use unix domain sockets
#include <string.h> // to use strcpy

#include "KVS-lib-MACROS.h"

#define RCV_QUERY_SUCCESS 0
#define RCV_QUERY_COM_ERROR -100
int rcvQueryKVSLocalServer(int clientSock, int * msgId, char * str1, char * str2);

#endif