#include <stdio.h>
#include <stdlib.h>

// valgrind --leak-check=full -v ./client

#include "KVS-lib.h"
#include "KVSLib_ui.h"

void printCallback(char *key){
    char *value = NULL;
    get_value(key,&value);
    printf("Buedas ananases\n");
    printf("Callback function for key %s changed to %s!\n",key,value);
    fflush(stdout);
}

int main(void){
    char *args[MAX_ARGS];
    char *secret = NULL;
    int exit = 0;
    int aux;

    printUI();

    while(!exit){
        printf(">> ");

        switch(getCommand(args)){
            case CONNECT_DES:
                aux = establish_connection(args[0],args[1]);
                printf("Return: %d\n",aux);
                break;
            case PUT_DES:
                aux = put_value(args[0],args[1]);
                printf("Return: %d\n",aux);
                break;
            case GET_DES:
                aux = get_value(args[0],&secret);
                printf("Value: %s, Return: %d\n",secret,aux);
                break;
            case DEL_DES:
                aux = delete_value(args[0]);
                printf("Return: %d\n",aux);
                break;
            case CALLBACK_DES:
                aux = register_callback(args[0],&printCallback);
                break;
            case CLOSE_DES:
                aux = close_connection(args[0],args[1]);
                printf("Return: %d\n",aux);
                break;
            case EXIT_DES:
                printf("Exiting\n");
                exit = 1;
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

        free(secret);
        secret = NULL;
    }

    return 0;
}