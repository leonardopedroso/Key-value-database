#include <stdio.h>
#include <stdlib.h>
#include <signal.h> // to manage signals for client disconnection
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#define PORT_NUM 50002  // Server port number

#include "KVSAuthServer-data.h"

#define SV_IP INADDR_ANY