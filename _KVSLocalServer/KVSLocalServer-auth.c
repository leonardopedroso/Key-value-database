#include "KVS_local-auth_com.h"
#include "KVSLocalServer-auth.h"

#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <limits.h>

// Return code for success receiving
#define REC_OK 0
// Return value for time out on receiving an acknowledgement
#define REC_TIMER_OUT -1
// Return value for error in recvfrom
#define REC_RECEIVING -2
// Return value for invalid sender
#define REC_INVALID_SENDER -3
// Return value for impossible to receive
#define REC_IMPOSSIBLE -4
// Return value for error in sendto
#define REC_SENDING -5
// Seconds to verify in the timer
#define REC_TIMER_TIME_SEC 0
// Microsseconds to verify in the timer
#define REC_TIMER_TIME_MSEC 5e5
// Number of times to try to contact the server without receiving answer
#define REC_MAX_TRIES 5

// to make available to all functions what is the server address
struct sockaddr_in svaddr;
// to make available to all functions what is the client socket
int cfd;
// mutex to prevent the sending and receiving of several requests at the same
// time
pthread_mutex_t sendReceiveMutex = PTHREAD_MUTEX_INITIALIZER;
// number of present request
int numReq = INT_MIN;
// mutex to prevent asynchronous changes to numReq
pthread_mutex_t numReqMutex = PTHREAD_MUTEX_INITIALIZER;

int initCom(void){
    cfd = socket(AF_INET, SOCK_DGRAM, 0); // create server socket
    // Ignore socket disconnection signal from client
    // it is handled when read returns -1
    signal(SIGPIPE, SIG_IGN); 
    // Catch error creating reception socket
    if (cfd == -1){
        return ERR_CREATING_SOCK;
    }

    // sets the maximum time a socket waits for receiving
    struct timeval tv;
    tv.tv_sec = REC_TIMER_TIME_SEC;
    tv.tv_usec = REC_TIMER_TIME_MSEC;
    setsockopt(cfd,SOL_SOCKET,SO_RCVTIMEO,(void*)&tv,sizeof(tv));

    memset(&svaddr,0,sizeof(struct sockaddr_in));   // initializes address
    svaddr.sin_family = AF_INET; // set socket family type
    // sets the server IP address 
    if(inet_aton(SV_IP,&svaddr.sin_addr) == 0){ 
        return ERR_CONVERTING_IP;
    } 
    // chooses port and guarantees portability regarding endianness
    svaddr.sin_port = htons(PORT_NUM);

    return SUCCESS_SOCK;
}

int endCom(void){
    return close(cfd);
}

// \brief verifies if the sender is the server assuming the server global
// variable
// \param sender pointer to the address of the sender
// \return on correspondence, returns not zero. on non correspondence returns 0
int verifySender(struct sockaddr_in *sender){
    return sender->sin_addr.s_addr == svaddr.sin_addr.s_addr &&
            sender->sin_family == svaddr.sin_family &&
            sender->sin_port == svaddr.sin_port;
}

// \brief the lowest level of communication to the authentication server. this
// function sends and tries to receive aknowledges from the server
// \param req request to send
// \param ans answer to fill in
// \return one of the REC_ errors in the list of defines above
int sendReceive(REQUEST *req,ANSWER *ans){
    socklen_t len = sizeof(struct sockaddr_in);
    struct sockaddr_in sender;
    int aux;

    pthread_mutex_lock(&sendReceiveMutex);

    if(sendto(cfd,req,sizeof(REQUEST),0,(struct sockaddr*)&svaddr,
        sizeof(struct sockaddr_in))!= sizeof(REQUEST)){
        return REC_SENDING;
    }

    // to avoid leaving the function due to messages from invalid senders
    // while the messages received are from invalid senders it keeps on
    // receiving
    // moreover if the request id corresponds to the one of another request 
    // that means it is a duplicate so it is just ignored
    while(1){
        aux = recvfrom(cfd,ans,sizeof(ANSWER),0,(struct sockaddr*)&sender,
            &len);
    
        // if there was an error in the recvfrom
        if(aux == -1){
            // if the error was due to a receiving timeout
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                pthread_mutex_unlock(&sendReceiveMutex);
                return REC_TIMER_OUT;
            } else{
                pthread_mutex_unlock(&sendReceiveMutex);
                return REC_RECEIVING;
            }
        }else{
            // if the sender is valid
            if(verifySender(&sender)){
                // and the size is not one of an answer
                if(aux!=sizeof(ANSWER)){
                    pthread_mutex_unlock(&sendReceiveMutex);
                    return REC_RECEIVING;
                }
                // and the size is one of an answer
                else{
                    // if the answer id is the same as the request id
                    if(ans->id == req->id){
                        pthread_mutex_unlock(&sendReceiveMutex);
                        return REC_OK;
                    }
                }
            }
        }
    }
}

// \brief the second lowest level of communication to the authentication 
// server. this function sends and tries to receive aknowledges from the server
// up to REC_MAX_TRIES. it is possible to multiple instances of this function
// to be running at a same time in the local server but their tries are always
// sequential
// \param req request to send
// \param ans answer to fill in
// \return one of the REC_ errors in the list of defines above
int commAuthServer(REQUEST *req,ANSWER *ans){
    int aux = 0;
    int tries = 0;

    // so that there can never be two requests with the same number
    pthread_mutex_lock(&numReqMutex);
    req->id = numReq;
    numReq++;
    pthread_mutex_unlock(&numReqMutex);

    // we will send REC_MAX_TRIES requests to the authentication server
    // if it does not receive a acknowledge, it tries again. if it receives
    // it returns. if it runs into an error, it returns the error
    while(tries < REC_MAX_TRIES){
        aux = sendReceive(req,ans);

        if(aux == REC_TIMER_OUT){
            tries++;
        }else{
            return aux;
        }
    }
    
    // if it cannot receive it returns that info
    return REC_IMPOSSIBLE;
}

int authCreateGroup(char * group, char * secret){
    REQUEST req;
    ANSWER ans;

    // creates the request
    req.code = REQ_CREATE;
    strcpy(req.group,group);
    strcpy(req.secret,secret);
    
    // if it runs into an error, it returns it. if not just carries on with the
    // answer given from the authentication server
    switch(commAuthServer(&req,&ans)){
        case REC_IMPOSSIBLE:
            return AUTH_IMPOSSIBLE_SERVER;
        case REC_RECEIVING:
            return AUTH_RECEIVING;
        case REC_SENDING:
            return AUTH_SENDING;
    }

    // performs the correspondence between the codes from the answers and the
    // codes of this part of the project
    switch(ans.code){
        case ANS_OK:
            return AUTH_OK;
        case ANS_ALLOC_ERROR:
            return AUTH_ALLOC_ERROR;
        case ANS_GROUP_ALREADY_EXISTS:
            return AUTH_GROUP_ALREADY_EXISTS;
        default:
            return AUTH_INVALID;
    }
}

int authDeleteGroup(char *group){
    REQUEST req;
    ANSWER ans;

    // creates the request
    req.code = REQ_DELETE;
    strcpy(req.group,group);
    memset(req.secret,'\0',MAX_SECRET_LEN);
    
    // if it runs into an error, it returns it. if not just carries on with the
    // answer given from the authentication server
    switch(commAuthServer(&req,&ans)){
        case REC_IMPOSSIBLE:
            return AUTH_IMPOSSIBLE_SERVER;
        case REC_RECEIVING:
            return AUTH_RECEIVING;
    }

    // performs the correspondence between the codes from the answers and the
    // codes of this part of the project
    switch(ans.code){
        case ANS_OK:
            return AUTH_OK;
        case ANS_GROUP_DSN_EXIST:
            return AUTH_GROUP_DSN_EXIST;
        default:
            return AUTH_INVALID;
    }
}

int authGetSecret(char * group, char * secret){
    REQUEST req;
    ANSWER ans;

    // creates the request
    req.code = REQ_SECRET;
    strcpy(req.group,group);
    memset(req.secret,'\0',MAX_SECRET_LEN);
    
    // if it runs into an error, it returns it. if not just carries on with the
    // answer given from the authentication server
    switch(commAuthServer(&req,&ans)){
        case REC_IMPOSSIBLE:
            return AUTH_IMPOSSIBLE_SERVER;
        case REC_RECEIVING:
            return AUTH_RECEIVING;
    }

    // performs the correspondence between the codes from the answers and the
    // codes of this part of the project
    switch(ans.code){
        case ANS_OK:
            strcpy(secret,ans.secret);
            return AUTH_OK;
        case ANS_GROUP_DSN_EXIST:
            return AUTH_GROUP_DSN_EXIST;
        default:
            return AUTH_INVALID;
    }
}