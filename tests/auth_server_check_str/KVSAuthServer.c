#include "KVSAuthServer.h"

int main(void){
    // ---------- Setup server variables ----------
    int sfd; // fd of rcv socket
    struct sockaddr_in svaddr; // struct addr of sever socket
    PAIR *head = NULL;
    
    // ---------- Setup server socket ----------
    sfd = socket(AF_INET, SOCK_STREAM, 0); // create server socket
    // Ignore socket disconnection signal from client, which is handled when read returns -1
    signal(SIGPIPE, SIG_IGN); 
    // Catch error creating reception socket
    if (sfd == -1){
        perror("Error creating auth server socket\n");
        exit(-1);
    }
    printf("Auth server socket created\n");
    
    // ---------- Bind server socket ----------
    memset(&svaddr,0,sizeof(struct sockaddr_in));   // initializes address
    svaddr.sin_family = AF_INET; // set socket family type
    svaddr.sin_addr.s_addr = SV_IP; // sets the server IP address
    svaddr.sin_port = htons(PORT_NUM);  // chooses port and guarantees portability regarding endianness
    // Catch error binding socket to address
    if( bind(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_in)) == -1){
        perror("Error binding socket\n");
        exit(-1);
    }
    printf("Reception socket binded to IP %d, port %u\n", svaddr.sin_addr.s_addr, PORT_NUM);

    char group1[MAX_GROUP_LEN];
    memset(group1,'\0',MAX_GROUP_LEN);
    
    strcpy(group1,"");
    printf("%d\n",checkPairElem(group1));

    memset(group1,1,MAX_GROUP_LEN);
    printf("%d\n",checkPairElem(group1));

    close(sfd); // closes the socket and may try to complete any pending transmission
    
    return 0;
}