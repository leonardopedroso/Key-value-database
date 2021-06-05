#ifndef KVS_LOCAL_SERVER_BASE_H
#define KVS_LOCAL_SERVER_BASE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h> // to manipulate strings
#include <pthread.h> // to use threads
#include <sys/socket.h> // to use sockets
#include <sys/un.h> // to use unix domain sockets
#include <signal.h> // to manage signals for client disconnection
#include <time.h> // to get connection and disconnection time
#include <stdint.h>
#include <unistd.h> // to open, close, read

#include "KVS-lib-MACROS.h" // Shared MACROS
#include "KVS_local-lib_com.h"

#endif