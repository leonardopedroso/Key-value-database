#ifndef KVS_LOCAL_SERVER_CB_H
#define KVS_LOCAL_SERVER_CB_H

typedef struct callbackStruct{
    char * key;
    int cb_sock;
    int cb_id;

    struct callbackStruct * prox;
}CALLBACK;


#endif