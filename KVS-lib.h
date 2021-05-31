#ifndef KVS_LIB_H
#define KVS_LIB_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // to open, close, read
#include <sys/socket.h> // to use sockets
#include <sys/un.h> // to use unix domain sockets
#include <string.h> // to use strcpy
#include <signal.h>


// Define server parameters
#define KVS_LOCAL_SERVER_ADDR "/tmp/KVSLocalServer"

#define SUCCESS 0
#define ERROR_ACCSS_DENIED -1
#define ERROR_GROUP_DSNT_EXIST -2
#define ERROR_ALLOC -3

#define ERROR_CREATING_SOCK -100
#define ERROR_CONNECTION_SERVER -101
#define ERROR_COM_SERVER -102
#define ERROR_DISCONNECTED_SOCK -103
#define ERROR_ALRDY_CONNECTED_SOCK -104
#define ERROR_CALLBACK_SOCK -105

int establish_connection (char * group_id, char * secret);

int put_value(char * key, char * value);
int get_value(char * key, char ** value);
int delete_value(char * key);

int close_connection();



#endif