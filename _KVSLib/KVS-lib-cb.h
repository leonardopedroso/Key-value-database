#ifndef KVS_LIB_CB_H
#define KVS_LIB_CB_H

#define CB_SERVER_BACKLOG 1

//#define DEBUG_CALLBACK 

#define CALLBACK_ALLOC_ERROR -1

typedef struct callbackStruct{
    char * key;
    int cb_id;
    void (*cb_func)(char *);

    struct callbackStruct * prox;
}CALLBACK;

void * callbackServerThread(void * arg);
void callbackDisconnect();
void callbackClear();
int callbackAdd(char * key, void (*callback_function)(char *));
void * callbackWrapperThread(void * arg);

#endif