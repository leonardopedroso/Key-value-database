#include <signal.h> // to manage signals for client disconnection
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include "KVS_local-auth_com.h"

// Success
#define SUCCESS 0
// Could not create socket
#define ERR_SOCK_CREATE -1
// Could not convert IP
#define ERR_CONVERT_IP -2
// Could not bind socket
#define ERR_SOCK_BIND -3

// \brief Creates the authentication server socket
// \param sfd pointer to the file descriptor of the socket
// \param svaddr pointer to the address of the server
// \return on success, returns SUCCESS. On error creating socket, returns
// ERR_SOCK_CREATE. On error converting IP, returns ERR_CONVERT_IP. On error
// binding socket, returns ERR_SOCK_BIND
int createServerSocket(int *sfd,struct sockaddr_in *svaddr);