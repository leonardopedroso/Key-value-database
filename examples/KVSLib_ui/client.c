#include <stdio.h>
#include <stdlib.h>

// valgrind --leak-check=full -v ./client

#include "KVS-lib.h"
#include "KVSLib_ui.h"

int main(void){
    char *args[MAX_ARGS];

    printUI();

    while(1){
        printf(">> ");

        switch(getCommand(args)){
            case CONNECT_DES:
                printf("Connecting to group %s, secret %s\n",args[0],args[1]);
                break;
            case PUT_DES:
                printf("Putting value %s to key %s\n",args[0],args[1]);
                break;
            case GET_DES:
                printf("Getting value from key %s\n",args[0]);
                break;
            case DEL_DES:
                printf("Deleting key %s\n",args[0]);
                break;
            case CLOSE_DES:
                printf("Closing connection\n");
                exit(1);
                break;
            case INV_DES:
                break;
            default:
                break;
        }

        for(int i = 0; i < MAX_ARGS; i++){
            free(args[i]);
            args[i] = NULL;
        }
    }
}