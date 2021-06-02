#ifndef KVS_LIB_COM_H
#define KVS_LIB_COM_H

#define DEBUG_COM

#define QUERY_OK 0
#define QUERY_ERROR_DISCONNECTED_SOCK -1
#define QUERY_ACCSS_DENIED -2
#define QUERY_GROUP_DSN_EXIST -3
#define QUERY_ALLOC_ERROR -4
#define QUERY_KEY_DSNT_EXIST -5
#define QUERY_COM_ERROR -100
#define QUERY_AUTH_COM -200
int queryKVSLocalServer(int msgId, char * str1, char * str2, uint64_t len2, char ** str3, uint64_t * len3);

#endif