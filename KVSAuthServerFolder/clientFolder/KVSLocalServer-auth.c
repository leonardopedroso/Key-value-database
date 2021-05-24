#include "KVS_local-auth_com.h"
#include "KVSLocalServer-auth.h"

// Return code for the receiver function on an acknowledgment
#define REC_ACK 1
// Return code for the receiver function on successful creating the timer
#define REC_GOOD_TIMER -15
// Return code for the receiver function on time out
#define REC_TIMER_OUT -16
// Return code for the receiver function on an invalid sender
#define REC_INVALID_SENDER -17
// Time to verify in the timer in seconds
#define REC_TIMER_TIME 1
// Number of times to try to contact the server without receiving answer
#define REC_MAX_TRIES 3

// to make available to all functions what is the server address
struct sockaddr_in svaddr;
// to make available to all functions what is the client socket
int cfd;

// \brief holds the contents of a valid answer and a pipe descriptor to know
// where to signal completion
// \param ans it tries to receive an answer so it holds a pointer to an answer
// \param pipeFd pointer to the file descriptor of the pipe
struct receiverStruct{
    ANSWER *ans;
    int* pipeFd;
};

int initCom(void){
    cfd = socket(AF_INET, SOCK_DGRAM, 0); // create server socket
    // Ignore socket disconnection signal from client
    // it is handled when read returns -1
    signal(SIGPIPE, SIG_IGN); 
    // Catch error creating reception socket
    if (cfd == -1){
        return ERR_CREATING_SOCK;
    }

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

// \brief thread to try to receive an acknowledge from the server. when it is
// done signals its end in a pipe. it already does some processing on the
// values returned
// \param arg pointer to a struct receiverStruct to where put the outputs and
// receive the inputs
void *recThread(void *arg){
    socklen_t len = sizeof(struct sockaddr_in);
    struct sockaddr_in sender;
    struct receiverStruct *recOut = (struct receiverStruct *) arg;
    int aux;

    aux = recvfrom(cfd,recOut->ans,sizeof(ANSWER),0,
        (struct sockaddr*)&sender,&len);
    
    // if there was an error in the recvfrom
    if(aux == -1){
        aux = AUTH_RECEIVING;
    }else{
        // if the sender is valid
        if(verifySender(&sender)){
            // and the size is not one of an answer
            if(aux!=sizeof(ANSWER)){
                aux = AUTH_RECEIVING;
            }
            // and the size is one of an answer
            else{
                aux=REC_ACK;
            }
        }else{
            aux = REC_INVALID_SENDER;
        }
    }

    // if it runs into an error, it will simply not be able to write on the
    // pipe and the timer will win
    write(recOut->pipeFd[1],&aux,sizeof(int));

    pthread_exit(NULL);
}

// \brief handler to a timer. writes on a pipe that the timer has finished
// \param pipeSigVal contains a pointer to a pipe file descriptor
void timerHandler(union sigval pipeSigVal){
    int *pipeFd = (int*)pipeSigVal.sival_ptr;
    int aux = REC_TIMER_OUT;
    
    // if it runs into an error, it will simply not be able to write on the
    // pipe and the timer will win
    write(pipeFd[1],&aux,sizeof(int));
}

// \brief inits the receiver timer to get its specific handler, argument and 
// time
// \param timerId pointer to the descriptor of the timer
// \param pipeDes pointer to the descriptor of the pipe
// \param ts pointer to the time struct
int initReceiverTimer(timer_t *timerId,int *pipeDes, struct itimerspec *ts){
    struct sigevent sev;
    
    // creates a timer to prevent being waiting for bad messages
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = timerHandler;
    sev.sigev_value.sival_ptr = (void*)pipeDes;
    sev.sigev_notify_attributes = NULL;
    
    if(timer_create(CLOCK_REALTIME,&sev,timerId)!=0){
        return AUTH_TIMER;
    }
    
    // sets the time of the timer
    ts->it_interval.tv_nsec = 0;
    ts->it_interval.tv_sec = 0;
    ts->it_value.tv_nsec = 0;
    ts->it_value.tv_sec = REC_TIMER_TIME;

    return REC_GOOD_TIMER;
}

// \brief tries to receive an answer from the server. if in a given period of
// time it is not received finishes waiting for it and signals it via its
// return value. in order to do this time out, a timer is created and the
// receiving operation is done in a thread. then one waits for the completion
// of the first and after that returns what happened
// \param ans pointer to an answer to be filled in
int recAcknowledge(ANSWER *ans){
    int pipeDes[2];
    int aux;
    pthread_t recThreadId;
    timer_t timerId;
    struct itimerspec ts;
    struct receiverStruct recOut;

    // creates a pipe to where the timer or the receiver thread can write when
    // they are finished
    if(pipe(pipeDes) != 0){
        return AUTH_PIPE;
    }
    if(initReceiverTimer(&timerId,pipeDes,&ts)==AUTH_TIMER){
        return AUTH_TIMER;
    }

    // creates the receiver thread that will try to receive an acknowledgement
    // from the server and gives it a pipe to where signal completion
    recOut.ans = ans;
    recOut.pipeFd = pipeDes;
    if(pthread_create(&recThreadId, NULL,recThread,&recOut)!=0){
        return AUTH_THREAD;
    }
    
    // sets the timer to run
    if(timer_settime(timerId,0,&ts,NULL)!=0){
        return AUTH_TIMER_SET;
    }

    // expects for a write on the pipe from the receiver thread or the timer
    if(read(pipeDes[0],&aux,sizeof(int))!=sizeof(int)){
        // on error orderly closes the ends of the pipe
        close(pipeDes[0]);
        close(pipeDes[1]);
        return AUTH_READ;
    }
    // orderly closes the pipes since they are not needed anymore
    close(pipeDes[0]);
    close(pipeDes[1]);
    
    // if it was the thread that finished first
    if(aux!=REC_TIMER_OUT){
        // joins the thread
        if(pthread_join(recThreadId,NULL)!=0){
            return AUTH_JOIN;
        }
    // if it was the timer
    }else{
        // cancels the receiver thread
        if(pthread_cancel(recThreadId)!=0){
            return AUTH_CANCEL;
        }
    }

    return aux;
}

int authCreateGroup(char * group, char * secret){
    ANSWER ans;
    REQUEST req;
    int aux;
    int tries = 0;
    
    // we will send REC_MAX_TRIES requests to the authentication server
    // send one wait for REC_TIMER_TIME seconds and try again
    while(tries < REC_MAX_TRIES){
        // creates a create group request and sends it to the server
        req.code = REQ_CREATE;
        strcpy(req.group,group);
        strcpy(req.secret,secret);
        if(sendto(cfd,&req,sizeof(REQUEST),0,(struct sockaddr*)&svaddr,
                sizeof(struct sockaddr_in))!= sizeof(REQUEST)){
            return AUTH_SENDING;
        }

        aux = recAcknowledge(&ans);
        
        // for a time out receive, increases the number of completed tries
        if(aux == REC_TIMER_OUT){
            tries++;
        }
        // if it received an acknowledgement
        else if(aux==REC_ACK){
            // correspondence between answer codes and auth codes
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
        // if it was not an invalid send 
        else if(aux != REC_INVALID_SENDER){
            // returns the errors encountered
            return aux;
        }
    }
    
    return AUTH_IMPOSSIBLE_SERVER;
}

int authDeleteGroup(char *group){
    ANSWER ans;
    REQUEST req;
    int aux;
    int tries = 0;
    
    // we will send REC_MAX_TRIES requests to the authentication server
    // send one wait for REC_TIMER_TIME seconds and try again
    while(tries < REC_MAX_TRIES){
        // creates a create group request and sends it to the server
        req.code = REQ_DELETE;
        strcpy(req.group,group);
        if(sendto(cfd,&req,sizeof(REQUEST),0,(struct sockaddr*)&svaddr,
                sizeof(struct sockaddr_in))!= sizeof(REQUEST)){
            return AUTH_SENDING;
        }

        aux = recAcknowledge(&ans);
        
        // for a time out receive, increases the number of completed tries
        if(aux == REC_TIMER_OUT){
            tries++;
        }
        // if it received an acknowledgement
        else if(aux==REC_ACK){
            // correspondence between answer codes and auth codes
            switch(ans.code){
                case ANS_OK:
                    return AUTH_OK;
                case ANS_GROUP_DSN_EXIST:
                    return AUTH_GROUP_DSN_EXIST;
                default:
                    return AUTH_INVALID;
            }
        }
        // if it was not an invalid send 
        else if(aux != REC_INVALID_SENDER){
            // returns the errors encountered
            return aux;
        }
    }
    
    return AUTH_IMPOSSIBLE_SERVER;
}

int authGetSecret(char * group, char ** secret){
    ANSWER ans;
    REQUEST req;
    int aux;
    int tries = 0;
    
    // we will send REC_MAX_TRIES requests to the authentication server
    // send one wait for REC_TIMER_TIME seconds and try again
    while(tries < REC_MAX_TRIES){
        // creates a create group request and sends it to the server
        req.code = REQ_SECRET;
        strcpy(req.group,group);
        if(sendto(cfd,&req,sizeof(REQUEST),0,(struct sockaddr*)&svaddr,
                sizeof(struct sockaddr_in))!= sizeof(REQUEST)){
            return AUTH_SENDING;
        }

        aux = recAcknowledge(&ans);
        
        // for a time out receive, increases the number of completed tries
        if(aux == REC_TIMER_OUT){
            tries++;
        }
        // if it received an acknowledgement
        else if(aux==REC_ACK){
            // correspondence between answer codes and auth codes
            switch(ans.code){
                case ANS_OK:
                    strcpy(*secret,ans.secret);
                    return AUTH_OK;
                case ANS_GROUP_DSN_EXIST:
                    return AUTH_GROUP_DSN_EXIST;
                default:
                    return AUTH_INVALID;
            }
        }
        // if it was not an invalid send 
        else if(aux != REC_INVALID_SENDER){
            // returns the errors encountered
            return aux;
        }
    }
    
    return AUTH_IMPOSSIBLE_SERVER;
}