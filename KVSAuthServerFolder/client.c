#include "KVS_local-auth_com.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(void){
    struct sockaddr_in svaddr;
    int cfd;
    
    cfd = socket(AF_INET, SOCK_DGRAM, 0); // create server socket
    // Ignore socket disconnection signal from client, which is handled when read returns -1
    signal(SIGPIPE, SIG_IGN); 
    // Catch error creating reception socket
    if (cfd == -1){
        perror("Error creating auth server socket\n");
        exit(-1);
    }
    printf("Auth client socket created\n");

    memset(&svaddr,0,sizeof(struct sockaddr_in));   // initializes address
    svaddr.sin_family = AF_INET; // set socket family type
    if(inet_aton(SV_IP_CL,&svaddr.sin_addr) == 0){ // sets the server IP address 
        perror("Error converting IP\n");
        exit(-1);
    } 
    svaddr.sin_port = htons(PORT_NUM);  // chooses port and guarantees portability regarding endianness

    REQUEST req;
    ANSWER ans;
    
    req.code = REQ_CREATE;
    strcpy(req.group,"Ola");
    strcpy(req.secret,"Estupido");
    if(sendto(cfd,&req,sizeof(REQUEST),0,(struct sockaddr*) &svaddr,sizeof(struct sockaddr_in))
            != sizeof(REQUEST)){
        printf("Merda\n");
    }

    if(read(cfd,&ans,sizeof(ANSWER)) != sizeof(ANSWER)){
        printf("Merda\n");
    } else{
        printf("%d\n",ans.code);
    }

    req.code = REQ_SECRET;
    if(sendto(cfd,&req,sizeof(REQUEST),0,(struct sockaddr*) &svaddr,sizeof(struct sockaddr_in))
            != sizeof(REQUEST)){
        printf("Merda\n");
    }

    if(read(cfd,&ans,sizeof(ANSWER)) != sizeof(ANSWER)){
        printf("MErda\n");
    } else{
        printf("%d\n",ans.code);
    }

    req.code = REQ_DELETE;
    if(sendto(cfd,&req,sizeof(REQUEST),0,(struct sockaddr*) &svaddr,sizeof(struct sockaddr_in))
            != sizeof(REQUEST)){
        printf("Merda\n");
    }

    if(read(cfd,&ans,sizeof(ANSWER)) != sizeof(ANSWER)){
        printf("MErda\n");
    } else{
        printf("%d\n",ans.code);
    }

    close(cfd);

    return 0;
}