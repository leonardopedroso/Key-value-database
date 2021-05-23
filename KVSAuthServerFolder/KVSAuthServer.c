#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "KVSAuthServer-com.h"
#include "KVSAuthServer-data.h"

int main(void){
    int sfd;                    // fd of server socket
    struct sockaddr_in svaddr;  // struct addr of sever socket
    
    switch(createServerSocket(&sfd,&svaddr)){
        case SUCCESS:
            printf("Authentication server socket created\n");
            printf("Socket binded to IP %s, port %u\n\n",SV_IP,PORT_NUM);
            break;
        case ERR_SOCK_CREATE:
            printf("Error creating socket\nShutting down\n");
            break;
        case ERR_CONVERT_IP:
            printf("Error converting socket IP\nShutting down\n");
            break;
        case ERR_SOCK_BIND:
            printf("Error binding socket\nShutting down\n");
            break;
    }

    PAIR *head = NULL;
    REQUEST req;
    ANSWER ans;
    int aux;
    struct sockaddr_in claddr;
    socklen_t len;

    while(1){
        // before usign recvfrom len shall always be initalized
        len = sizeof(struct sockaddr_in);
        aux = recvfrom(sfd,&req,sizeof(REQUEST),0,
            (struct sockaddr*)&claddr,&len);
        // if an error happens it will most likely be irreversible so we
        // shutdown the server
        if(aux == -1){
            printf("Error in recvfrom\nShutting down\n");
            break;
        }
        if(aux != sizeof(REQUEST)){
            printf("Invalid size datagram received\nIgnoring datagram\n");
            continue;
        }

        switch(req.code){
            case REQ_CREATE:
                printf("Received a request to add pair from IP %s, port %u\n",
                    inet_ntoa(claddr.sin_addr),ntohs(claddr.sin_port));
                ans.code = addPair(&head,req.group,req.secret);
                break;
            case REQ_DELETE:
                printf("Received a request to delete pair from IP %s,"
                    " port %u\n",inet_ntoa(claddr.sin_addr),
                    ntohs(claddr.sin_port));
                ans.code = deletePair(&head,req.group);
                break;
            case REQ_SECRET:
                printf("Received a request to get secret from IP %s,"
                    " port %u\n",inet_ntoa(claddr.sin_addr),
                    ntohs(claddr.sin_port));
                ans.code = getSecret(head,req.group,ans.secret);
                break;
            default:
                printf("Received an invalid request from IP %s, port %u\n",
                    inet_ntoa(claddr.sin_addr),
                    ntohs(claddr.sin_port));
                ans.code = REQ_CODE_INV;
                break;
        }

        switch(ans.code){
            case PAIR_ALRD_EXISTS:
                printf("Pair with given group name already exists\n");
                break;
            case PAIR_ALLOC_ERR:
                printf("Could not allocate memory for new pair\n");
                break;
            case GROUP_DSNT_EXIST:
                printf("Pair with given group name does not exist\n");
                break;
            case BAD_GROUP:
                printf("Given group name is not valid\n");
                break;
            case BAD_SECRET:
                printf("Given secret is not valid\n");
                break;
        }

        if(ans.code == SUCCESS_GET_SECRET){
            ans.code = SUCCESS;
            aux = sendto(sfd,&ans,sizeof(ANSWER),0,(struct sockaddr*)&claddr,
                sizeof(struct sockaddr_in));
        } else if(ans.code != REQ_CODE_INV){
            memset(ans.secret,'\0',MAX_SECRET_LEN);
            aux = sendto(sfd,&ans,sizeof(ANSWER),0,(struct sockaddr*)&claddr,
                sizeof(struct sockaddr_in));
        }

        if(aux == -1){
            if(errno == ENOBUFS){
                printf("Queue complete\n");
            } else if(errno == ENOMEM){
                printf("No memory available\n");
            } else{
                printf("Error in sendto\nShutting down\n");
                break;
            }
        } else if(aux != sizeof(ANSWER)){
            printf("Could not send the complete message\n");
        }
    }

    deleteAllPairs(&head);
    // closes the socket and completes any pending transmission
    close(sfd);
    
    return 0;
}