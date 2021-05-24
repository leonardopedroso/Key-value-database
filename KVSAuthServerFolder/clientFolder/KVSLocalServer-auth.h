#ifndef KVS_LOCAL_SERVER_AUTH_H
#define KVS_LOCAL_SERVER_AUTH_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

#include "KVS-lib-MACROS.h"

// Success creating the socket
#define SUCCESS_SOCK 0
// Error creating the socket
#define ERR_CREATING_SOCK -1
// Error converting the IP of the server
#define ERR_CONVERTING_IP -2
// \brief initalizes a global socket and server address
int initCom(void);

// \brief closes a global socket
// \return on success, returns 0. on error, returns -1. to check the error
// use errno taking into consideration that the error was on a call to close
int endCom(void);

// Return code for success
#define AUTH_OK 0
// Return code for group not existent
#define AUTH_GROUP_DSN_EXIST -1
// Return code for allocation error
#define AUTH_ALLOC_ERROR -2
// Return code for group already existent
#define AUTH_GROUP_ALREADY_EXISTS -3
// Return code for impossibility of reaching the server
#define AUTH_IMPOSSIBLE_SERVER -4
// Return code for error sending datagram
#define AUTH_SENDING -5
// Return code for error receiving datagram
#define AUTH_RECEIVING -6
// Return code for invalid code received
#define AUTH_INVALID -7
// Return code for pipe not well created
#define AUTH_PIPE -8
// Return code for thread not well created
#define AUTH_THREAD -9
// Return code for error in reading from pipe
#define AUTH_READ -10
// Return code for error in joining receiver thread
#define AUTH_JOIN -11
// Return code for error in cancel
#define AUTH_CANCEL -12
// Return code for error creating timer
#define AUTH_TIMER -13
// Return code for error setting time in timer
#define AUTH_TIMER_SET -14

// \brief creates a group from a name and a secret. should be preceded by a
// call to initCom
// \param group name of the group to be created
// \param secret secret string of the group to be created
// \return on success, returns AUTH_OK. on allocation error in the 
// authentication server, returns AUTH_ALLOC_ERROR. on group already existent,
// returns AUTH_GROUP_ALREADY_EXISTS. on invalid group or secret returns
// respectively AUTH_INVALID_GROUP or AUTH_INVALID_SECRET. on the impossibility
// of reaching the server, returns 
int authCreateGroup(char * group, char * secret);

int authDeleteGroup(char * group);

// \brief gets a secret of a given group from the authentication server. 
// should be preceded by a call to initCom
// \param group name of the group
// \param secret pointer to memory to where the secret should be copied
int authGetSecret(char * group, char ** secret);

#endif