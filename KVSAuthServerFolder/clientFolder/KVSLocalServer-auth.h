#ifndef KVS_LOCAL_SERVER_AUTH_H
#define KVS_LOCAL_SERVER_AUTH_H

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

/* 
All the following functions mediate communications between the local and the
authentication servers. Given the communication is made through datagrams, the 
protocol used is the UDP. This protocol is not reliable. Datagrams sent via it
may not arrived in the same order they were sent, may be duplicated and may not
arrive at all as stated in "The Linux Programming Interface" by Kerrisk. 

To solve these problems we implemented a simple handshake protocol in which the
local server only assumes the request he made to the authentication server was
actually followed and executed if it receives an acknowledgmenet from the
authentication server informing of the success of the operation. The local
server tries to get an aknowledgment of this sort or a corresponding error for
a specific number of times before it gives up on contacting the server. These
operations are made one at a time for each request of the local server to the
authentication server through the use of a mutex.

The lack of ordering of messages between different requests is not a problem
given we take care of one request at a time.

The possibility of a message not reaching the auth server from the local server
is not a problem neither because the local server can try to send it again.

The possibility of duplication of messages from the auth to the local is taken
care by using a request id. After the first duplicate message is received, the
next request will have a differente id so when it reaches the receive phase it
will just ignore the pending previous duplicate message and wait for the one
with the same id of its request.

The possibility of a message not reaching the local server from the auth server
is taken care by the previous acknowledge system, the structure of the
functions, and by a change in the create group operation inside the auth
server. First, observe that for the deleteGroup and getSecret functions
this is not a problem since if the message does not reach the local server it
will just ask for it again and it will just get the secret again or for the
deleteGroup function it can in the maximum receive that it is already non
existent which in fact is not a problem for the function. Second, we solved
the problem for the createGroup function by opening in the auth server the
possibility of sending a success creating a group by just trying to create one
with the name and the secret of other already existent. This is if you try to
create a group, the auth server creates it but it does not send an acknowledge
the local server will ask to create it again, the auth server will check it is
already created with the same secret and it will signal to the local that it
was created despite the fact it was created because of a previous request. The
only problem of this implementation is if two local servers try to sequentially
create two groups with the same secret. If this happens both will think they
own the group and may delete it, restricting the access of the other to it.
However, since the secrets are randomly created as MAX_STR_LEN strings with
255 characters, this possibility is incredibly small, so it is not taken into
account. Besides, even if it happened, the program would not crash and other
procedures were created to deal with a situation of this type.
*/

// \brief creates a group from a name and a secret. should be preceded by a
// call to initCom
// \param group name of the group to be created
// \param secret secret string of the group to be created
// \return may return AUTH_OK, AUTH_GROUP_ALRD_EXISTS, AUTH_ALLOC_ERROR,
// AUTH_IMPOSSIBLE_SERVER, AUTH_SENDING, AUTH_RECEIVING, and AUTH_INVALID
int authCreateGroup(char * group, char * secret);

// \brief deletes a group from a name and a secret. should be preceded by a
// call to initCom
// \param group name of the group to be created
// \param secret secret string of the group to be created
// \return may return AUTH_OK, AUTH_GROUP_DSN_EXIST, AUTH_IMPOSSIBLE_SERVER, 
// AUTH_SENDING, AUTH_RECEIVING, and AUTH_INVALID
int authDeleteGroup(char * group);

// \brief gets a secret of a given group from the authentication server. 
// should be preceded by a call to initCom
// \param group name of the group
// \param secret pointer to memory to where the secret should be copied
// \return may return AUTH_OK, AUTH_GROUP_DSN_EXIST, AUTH_IMPOSSIBLE_SERVER, 
// AUTH_SENDING, AUTH_RECEIVING, and AUTH_INVALID
int authGetSecret(char * group, char ** secret);

#endif