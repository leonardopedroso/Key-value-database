#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "KVSLib_ui.h"
#include "KVS-lib-MACROS.h"

void printUI(void){
    printf("\n");
    printf("Group id and secret up to %d characters\n",MAX_GROUP_LEN-1);
    printf("Possible commands:\n");
    printf(CONNECT_CMD ARG_DELIM "[<groupID>]" ARG_DELIM "[<secret>]"
        "\tConnects to a KVSLocalServer giving the secret\n");
    printf(PUT_CMD ARG_DELIM "[<key>]" ARG_DELIM "[<value>]"
        "\t\tRemoves the group and all associated data\n");
    printf(GET_CMD ARG_DELIM "[<key>]" ARG_DELIM "\t\t\tGet the value of a key\n");
    printf(CALLBACK_CMD ARG_DELIM "[<key>]" ARG_DELIM "\t\tRegister callback\n");
    printf(DEL_CMD " [<key>] \t\t\tDelete key-value pair\n");
    printf(CLOSE_CMD "\t\t\t\tCloses the connection\n");
    printf(EXIT_CMD "\t\t\t\tExits this UI\n");
}

// \brief Gets a string from the keyboard, handles the errors and prints them
// and takes the new line character from it
// \param input unallocated char pointer to pass the string through
// \param len length of the string to be taken from the keyboard
// \return On success, returns 0 and, on error on fgets, returns ERR_FGETS. On
// input too large, ERR_TOO_LARGE. On memory allocation error, return ERR_ALLOC
int getStrFromStdin(char **input){
    // allocates memory
    *input = (char *)calloc(MAX_INPUT,sizeof(char));
    if(*input == NULL){
        printf("Error: Error allocating input\n");
        return ERR_ALLOC;
    }

    // initializes the string
    memset(*input,'\0',MAX_INPUT);

    // gets input from user
    if(fgets(*input,MAX_INPUT,stdin)==NULL){
        // on error, frees input
        free(*input);
        printf("Error: Error on fgets\n");
        return ERR_FGETS;
    }
    // If it does not find the new line character, it clears completely stdin
    if(memchr(*input,'\n',MAX_INPUT) == NULL){
        while(getchar() != '\n');
        free(*input);
        printf("Error: Input too large\n");
        return ERR_TOO_LARGE;
    }
    // the last char of input must be \n or \0 if MAX_INPUT is respected
    (*input)[strlen(*input)-1] = '\0';

    return SUCCESS;
}

int getCommandStrs(char *input,char **command,char **args,int *nArgs){
    char *auxStr,*savePtr;

    // initialize variables
    *nArgs = 0;
    *command = NULL;
    for(int i=0; i<MAX_ARGS;i++){
        args[i] = NULL;
    }

    // gets the command string
    auxStr = strtok_r(input,ARG_DELIM,&savePtr);
    if(auxStr == NULL){
        return EMPTY;
    }else{
        *command = (char *)calloc(strlen(auxStr)+1,sizeof(char));
        if(*command == NULL){
            printf("Error allocating command\n");
            return ERR_ALLOC;
        }
        strcpy(*command,auxStr);
    }

    // gets the args strings
    for(int i = 0; i < MAX_ARGS; i++){
        auxStr = strtok_r(NULL,ARG_DELIM,&savePtr);
        if(auxStr!=NULL){
            args[i] = (char *)calloc(strlen(auxStr)+1,sizeof(char));
            if(args[i] == NULL){
                printf("Error allocating arg\n");
                return ERR_ALLOC;
            }
            strcpy(args[i],auxStr);
            (*nArgs)++;
        }else{
            break;
        }
    }

    // searches for any more arg
    if(*nArgs == MAX_ARGS){
        auxStr = strtok_r(NULL,ARG_DELIM,&savePtr);
        if(auxStr != NULL){
            *nArgs = MAX_ARGS + 1;
        }
    }

    return SUCCESS;
}

int promptInvNArgs(char *command,int des,int nArgs,int neededNArgs){
    if(nArgs < neededNArgs){
        printf("%s: Too few arguments\n",command);
        return INV_DES;
    }else if(nArgs > neededNArgs){
        printf("%s: Too many arguments\n",command);
        return INV_DES;
    }

    return des;
}

int getCommand(char **args){
    char *input, *command;
    int aux, nArgs, des;

    // initializes the args
    for(int i=0; i<MAX_ARGS;i++){
        args[i] = NULL;
    }

    // gets input from stdin allocating it in the heap
    aux = getStrFromStdin(&input);
    if(aux!=SUCCESS){
        return aux;
    }

    aux = getCommandStrs(input,&command,args,&nArgs);
    if(aux != SUCCESS){
        // frees all the memory except for the args that are assumed to be free
        // outside
        free(input);
        free(command);
        return aux;
    }

    if(strcmp(command,CONNECT_CMD) == 0){
        des = promptInvNArgs(CONNECT_CMD,CONNECT_DES,nArgs,2);
    }else if(strcmp(command,PUT_CMD) == 0){
        des = promptInvNArgs(PUT_CMD,PUT_DES,nArgs,2);
    }else if(strcmp(command,GET_CMD) == 0){
        des = promptInvNArgs(GET_CMD,GET_DES,nArgs,1);
    }else if(strcmp(command,DEL_CMD) == 0){
        des = promptInvNArgs(DEL_CMD,DEL_DES,nArgs,1);
    }else if(strcmp(command,CALLBACK_CMD) == 0){
        des = promptInvNArgs(CALLBACK_CMD,CALLBACK_DES,nArgs,1);
    }else if(strcmp(command,CLOSE_CMD) == 0){
        des = promptInvNArgs(CLOSE_CMD,CLOSE_DES,nArgs,0);
    }else if(strcmp(command,EXIT_CMD) == 0){
        des = promptInvNArgs(EXIT_CMD,EXIT_DES,nArgs,0);
    }else{
        printf("%s: Invalid command\n",command);
        des = INV_DES;
    }

    free(input);
    free(command);
    return des;
}