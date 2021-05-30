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
            fprintf(stderr,"Authentication server socket created\n");
            printf("Socket binded to IP %s, port %u\n",SV_IP,PORT_NUM);
            break;
        case ERR_SOCK_CREATE:
            fprintf(stderr,"Error creating socket\nShutting down\n");
            return 0;
        case ERR_CONVERT_IP:
            fprintf(stderr,"Error converting socket IP\nShutting down\n");
            return 0;
        case ERR_SOCK_BIND:
            fprintf(stderr,"Error binding socket\nShutting down\n");
            return 0;
    }
    printf("\n");

    PAIR *head = NULL;  // head of the list of PAIRS
    REQUEST req;
    ANSWER ans;
    int aux;
    struct sockaddr_in claddr;
    socklen_t len;

    while(1){
        // before usign recvfrom len shall always be initalized
        len = sizeof(struct sockaddr_in);
        // receives a request from a client
        aux = recvfrom(sfd,&req,sizeof(REQUEST),0,
            (struct sockaddr*)&claddr,&len);
        // if an error happens while receiving we shutdown the server
        if(aux == -1){
            fprintf(stderr,"Error in recvfrom\nShutting down\n");
            break;
        }
        if(aux != sizeof(REQUEST)){
            fprintf(stderr,"Invalid size datagram received\nIgnoring\n");
            continue;
        }

        // handles the request based on its code
        switch(req.code){
            case REQ_CREATE:
                printf("Add pair request from   %s, port %u\n",
                    inet_ntoa(claddr.sin_addr),ntohs(claddr.sin_port));
                printf("Group -  %s\n",req.group);
                printf("Secret - %s\n",req.secret);
                aux = addPair(&head,req.group,req.secret);
                break;
            case REQ_DELETE:
                printf("Delete pair request from %s, port %u\n",
                    inet_ntoa(claddr.sin_addr),ntohs(claddr.sin_port));
                printf("Group -  %s\n",req.group);
                aux = deletePair(&head,req.group);
                break;
            case REQ_SECRET:
                printf("Get secret request from  %s, port %u\n",
                    inet_ntoa(claddr.sin_addr),
                    ntohs(claddr.sin_port));
                printf("Group -  %s\n",req.group);
                aux = getSecret(head,req.group,ans.secret);
                break;
            default:
                printf("Invalid request from     %s, port %u\n",
                    inet_ntoa(claddr.sin_addr),ntohs(claddr.sin_port));
                aux = REQ_CODE_INV;
                break;
        }

        // fills in an answer based on the return values of the functions
        switch(aux){
            case SUCCESS:
                ans.code = ANS_OK;
                printf("Success\n");
                break;
            case PAIR_ALRD_EXISTS:
                fprintf(stderr,"Group name already exists\n");
                ans.code = ANS_GROUP_ALREADY_EXISTS;
                break;
            case PAIR_ALLOC_ERR:
                fprintf(stderr,"Memory allocation error\n");
                ans.code = ANS_ALLOC_ERROR;
                break;
            case GROUP_DSNT_EXIST:
                fprintf(stderr,"Group name does not exist\n");
                ans.code = ANS_GROUP_DSN_EXIST;
                break;
            case BAD_GROUP:
                fprintf(stderr,"Group name is not valid\n");
                break;
            case BAD_SECRET:
                fprintf(stderr,"Secret is not valid\n");
                break;
        }

        // sends the acknowledge if it got a valid request
        if(aux != REQ_CODE_INV && aux != BAD_GROUP && aux != BAD_SECRET){
            ans.id = req.id;
            aux = sendto(sfd,&ans,sizeof(ANSWER),0,(struct sockaddr*)&claddr,
                sizeof(struct sockaddr_in));
            if(aux == -1){
                fprintf(stderr,"Errors in sending a datagram\n");
            } else if(aux != sizeof(ANSWER)){
                fprintf(stderr,"Could not send the complete datagram\n");
            }
        }

        // to divide the log of each request
        printf("\n");
    }

    deleteAllPairs(&head);
    close(sfd);
    
    return 0;
}