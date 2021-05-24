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
    
    printf("%d\n",authCreateGroup("Hello","Ola"));
    printf("%d %s\n",authGetSecret("Hello",&secret),secret);
    printf("%d\n",authDeleteGroup("Hello"));

    printf("%d\n",authDeleteGroup("Hello"));
    printf("%d %s\n",authGetSecret("Hello",&secret),secret);

    printf("%d\n",authCreateGroup("estupido","ze"));
    
    free(secret);
    endCom();

    return 0;
}