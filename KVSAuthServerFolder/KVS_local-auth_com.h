#ifndef KVS_LOCAL_AUTH_COM_H
#define KVS_LOCAL_AUTH_COM_H

#include "KVS-lib-MACROS.h"

// Server address
#define SV_IP "172.18.0.1"
// Server port number
#define PORT_NUM 50002

// Request code for asking the secret from the server
#define REQ_SECRET 0
// Request code for asking the server to create a group
#define REQ_CREATE 1
// Request code for asking the server to delete a group
#define REQ_DELETE 2

typedef struct requestStruct{
    // action code
    int code;
    // name of the group
    char group[MAX_GROUP_LEN];
    // secret of the group
    char secret[MAX_SECRET_LEN];
} REQUEST;

// Answer code for success
#define ANS_OK 0
// Answer code for group not existent
#define ANS_GOUP_DSN_EXIST -1
// Answer code for allocation error
#define ANS_ALLOC_ERROR -2
// Answer code for group already existent
#define ANS_GROUP_ALREADY_EXISTS -3
// Answer code for invalid string
#define ANS_INVALID_STR -4
// Answer code for other errors
#define ANS_OTHER_ERR -5

typedef struct answerStruct{
    // answer code
    int code;
    // secret
    char secret[MAX_SECRET_LEN];
} ANSWER;

#endif