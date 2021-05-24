#ifndef KVS_LOCAL_SERVER_AUTH_H
#define KVS_LOCAL_SERVER_AUTH_H

#define AUTH_OK 0
#define AUTH_GOUP_DSN_EXIST -1
#define AUTH_ALLOC_ERROR -2
#define AUTH_GROUP_ALREADY_EXISTS -3

#define AUTH_COM_ERROR -100

int authGetSecret(char * group, char ** secret);

int authCreateGroup(char * group, char ** secret);

int authDeleteGroup(char * group);


#endif