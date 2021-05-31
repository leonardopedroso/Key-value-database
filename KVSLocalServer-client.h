#ifndef KVS_LOCAL_SERVER_CLIENT_H
#define KVS_LOCAL_SERVER_CLIENT_H

#include "KVSLocalServer-base.h"
#include "KVSLocalServer-data.h" // to manage data

// ---------- Server and client management prototypes ----------
#define SUCCESS_CLIENT_HANDLE 0
#define ERROR_CLIENT_ALLOCATION -1
#define ERROR_CLIENT_MUX -2

int clientHandle(int sockClient);
int clientAdd(CLIENT * client);
int clientAuth(CLIENT * client, char * groupId, char * secret);
int clientDisconnect(CLIENT * client);
int clientShow();
void closeClients();


#endif