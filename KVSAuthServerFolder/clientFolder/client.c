#include <stdio.h>
#include <stdlib.h>

#include "KVSLocalServer-auth.h"

int main(void){
    switch(initCom()){
        case SUCCESS_SOCK:
            fprintf(stderr,"Communication with auth server can start\n");
            break;
        case ERR_CREATING_SOCK:
            fprintf(stderr,"Could not create socket\n");
            return 0;
        case ERR_CONVERTING_IP:
            fprintf(stderr,"Could not convert IP\n");
            return 0;
    }

    char *secret;
    secret = (char*)calloc(MAX_SECRET_LEN,sizeof(char));
    
    printf("%d\n",authCreateGroup("ana","mae"));
    printf("%d\n",authCreateGroup("ana","mae"));
    printf("%d\n",authCreateGroup("ana","temari"));
    
    printf("%d %s\n",authGetSecret("ana",secret),secret);
    printf("%d\n",authDeleteGroup("ana"));
    printf("%d %s\n",authGetSecret("ana",secret),secret);
    
    printf("%d\n",authDeleteGroup("ze"));
    printf("%d\n",authCreateGroup("ze","filho"));
    printf("%d\n",authDeleteGroup("ze"));
    
    free(secret);
    endCom();

    return 0;
}