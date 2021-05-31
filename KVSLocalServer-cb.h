#ifndef KVS_LOCAL_SERVER_CB_H
#define KVS_LOCAL_SERVER_CB_H

#include "KVSLocalServer-base.h"
#include "KVSLocalServer-data.h"

typedef struct callbackStruct{
    char * key;
    int cb_sock;
    int cb_id;

    struct callbackStruct * prox;
}CALLBACK;

void callbackConnect(CLIENT * client);

#endif
