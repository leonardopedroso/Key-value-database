enum KVS_LIB_STATUS{
SUCCESS = 0,
ERROR_ACCSS_DENIED = -1,
ERROR_GROUP_DSNT_EXIST = -2,
ERROR_ALLOC = -3,
ERROR_KEY_DSNT_EXIST = -4,
ERROR_INVALID_ARG_LEN = -5,
ERROR_CREATING_SOCK = -100,
ERROR_CONNECTION_SERVER = -101,
ERROR_COM_SERVER = -102,
ERROR_DISCONNECTED_SOCK = -103,
ERROR_ALRDY_CONNECTED_SOCK = -104,
ERROR_CALLBACK_SOCK = -105,
ERROR_CALLBACK_COM_ERROR = -106,
ERROR_COM_AUTH_SERVER = -200
};
int establish_connection (char * group_id, char * secret);
int put_value(char * key, char * value);
int get_value(char * key, char ** value);
int delete_value(char * key);
int register_callback(char * key, void (*callback_function)(char *));
int close_connection();