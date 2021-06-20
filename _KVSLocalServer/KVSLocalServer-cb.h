#ifndef KVS_LOCAL_SERVER_CB_H
#define KVS_LOCAL_SERVER_CB_H

#include "KVSLocalServer-base.h"
#include "KVSLocalServer-data.h"

//#define DEBUG_CALLBACK

typedef struct callbackStruct{
    char * key;
    char * group_id;
    int cb_sock;
    int cb_id;

    struct callbackStruct * prox;
}CALLBACK;

void callbackConnect(CLIENT * client);
int callbackRegister(CLIENT* client, char * key, char * cb_id);

void callbackDeleteKey(char * key, char * group_id);
void callbackDeleteClient(int cb_sock);
void callbackFlag(char * key, char * group_id);

#endif
