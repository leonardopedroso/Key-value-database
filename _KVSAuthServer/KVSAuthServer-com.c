#include "KVS_local-auth_com.h"
#include "KVSAuthServer-com.h"

int createServerSocket(int *sfd,struct sockaddr_in *svaddr){
    *sfd = socket(AF_INET, SOCK_DGRAM, 0); // create server socket
    // Ignore socket disconnection signal from client, which is handled when 
    // read returns -1
    signal(SIGPIPE, SIG_IGN); 
    // Catch error creating reception socket
    if (*sfd == -1){
        return ERR_SOCK_CREATE;
    }
    
    memset(svaddr,0,sizeof(struct sockaddr_in));   // initializes address
    svaddr->sin_family = AF_INET; // set socket family type
    if(inet_aton(SV_IP,&(svaddr->sin_addr)) == 0){ // sets the server IP address 
        return ERR_CONVERT_IP;
    }
    // chooses port and guarantees portability regarding endianness
    svaddr->sin_port = htons(PORT_NUM);
    // Catch error binding socket to address
    if(bind(*sfd, (struct sockaddr *) svaddr, sizeof(struct sockaddr_in))==-1){
        return ERR_SOCK_BIND;
    }

    return SUCCESS;
}