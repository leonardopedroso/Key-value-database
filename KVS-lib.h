#ifndef KVS_LIB_H
#define KVS_LIB_H

#define SUCCESS 0
#define ERROR_ACCSS_DENIED -1
#define ERROR_GROUP_DSNT_EXIST -2
#define ERROR_ALLOC -3
#define ERROR_KEY_DSNT_EXIST -4

#define ERROR_CREATING_SOCK -100
#define ERROR_CONNECTION_SERVER -101
#define ERROR_COM_SERVER -102
#define ERROR_DISCONNECTED_SOCK -103
#define ERROR_ALRDY_CONNECTED_SOCK -104
#define ERROR_CALLBACK_SOCK -105
#define ERROR_CALLBACK_COM_ERROR -106

#define ERROR_AUTH_COM -200

int establish_connection (char * group_id, char * secret);
int put_value(char * key, char * value);
int get_value(char * key, char ** value);
int delete_value(char * key);
int register_callback(char * key, void (*callback_function)(char *));
int close_connection();

#endif