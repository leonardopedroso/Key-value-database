#ifndef KVS_LIB_H
#define KVS_LIB_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // to open, close, read
#include <sys/socket.h> // to use sockets
#include <sys/un.h> // to use unix domain sockets
#include <string.h> // to use strcpy

// Define server parameters
#define KVS_LOCAL_SERVER_ADDR "/tmp/KVSLocalServer"

#define ESTBL_CONN_SUCCESS 0
#define ESTBL_CONN_ACCSS_DENIED -1
#define ESTBL_CONN_ERROR_CREATING_SOCK -100
#define ESTBL_CONN_CONNECTION_SERVER -101
#define ESTBL_CONN_COM_SERVER -102
int establish_connection (char * group_id, char * secret);

#define CLOSE_CONN_SUCCESS 1
int close_connection();


#endif