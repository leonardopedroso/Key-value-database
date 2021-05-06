#include <sys/socket.h> // to use sockets
#include <sys/un.h> // to use unix domain sockets

// Define server parameters
#define KVS_LOCAL_SERVER_ADDR "/tmp/KVSLocalServer"

#define SUCCESS_ESTABLISH_CONNECTION 0
#define ERROR_CREATING_SOCK -1
#define ERROR_CONNECTION_SERVER -2
int establish_connection (char * group_id, char * secret);