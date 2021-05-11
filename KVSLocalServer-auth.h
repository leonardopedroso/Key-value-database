#ifndef KVS_LOCAL_SERVER_AUTH_H
#define KVS_LOCAL_SERVER_AUTH_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h> // to manipulate strings
#include "KVS-lib-MACROS.h"

// Podem retornar 
#define AUTH_OK 0
#define AUTH_GOUP_DSN_EXIST -1
#define AUTH_ALLOC_ERROR -2
#define AUTH_GROUP_ALREADY_EXISTS -3

#define AUTH_COM_ERROR -100

int authGetSecret(char * group, char ** secret);

int authBroadcastGroupAndSecret(char * group, char ** secret);


#endif