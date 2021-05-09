#include "ui.h"

void printMenu(void){
    printf("Possible commands:\n");
    printf("Group ID up to %d characters\n",MAX_GROUP_ID);
    printf("%s%s[<groupID>]\tCreates a new group with the group\n",CREATE_CMD,
        ARG_DELIM);
    printf("\t\t\tidentifier specified\n");
    printf("%s%s[<groupID>]\tRemoves the group and all associated data\n",
        DELETE_CMD,ARG_DELIM);
    printf("%s%s[<groupID>]\tPrints the secret and the number of key-value\n",
        GROUP_CMD,ARG_DELIM);
    printf("\t\t\tpairs of a group\n");
    printf("%s\t\t\tLists all currently and past connected applications,\n",
        APPS_CMD);
    printf("\t\t\tprinting their PID, connection establishing time and\n");
    printf("\t\t\tconnection close time (if not currently connected\n");
    printf("\n");
}

// \brief Gets a string from the keyboard, handles the errors and prints them
// and takes the new line character from it
// \param input string to pass the string through
// \param len length of the string to be taken from the keyboard
// \return On success, returns 0 and, on error, returns -1
int getStrFromStdin(char input[],int len){
    char *aux;

    if(fgets(input,len,stdin)==NULL){
        printf("Error in getting input\n");
        return -1;
    }
    // If it does not find the new line character, it clears completely stdin
    if(memchr(input,'\n',len) == NULL){
        while(getchar() != '\n');
        
        printf("Input too large\n");
        return -1;
    }
    // so that the new line character is not confused with part of an argument
    // one of the args used as an auxiliary variable
    // TODO
    aux = strchr(input, '\n');
    *aux = '\0';

    return 0;
}

int getCommand(char **groupName){
    char input[MAX_INPUT];
    char *arg[2], *savePtr;
    int nArgs = 0, aux = 0; //TODO

    memset(input,'\0',MAX_INPUT);
    *groupName = NULL;

    if(getStrFromStdin(input,MAX_INPUT)){
        return 0;
    }

    // gets the pointers to each token
    arg[0] = strtok_r(input,ARG_DELIM,&savePtr);
    arg[1] = strtok_r(NULL,ARG_DELIM,&savePtr);

    // if none is given
    if(arg[0] == NULL){
        printf("No argument given\n");
        return 0;
    } 
    else if(arg[1] == NULL){
        // only one arg given
        if(strcmp(APPS_CMD,arg[0]) == 0){
            return APPS_DES;
        }
    } 
    else if(strtok_r(NULL,ARG_DELIM,&savePtr) == NULL){
        // two args given
        // the second arg is the group name and cannot be bigger than MAX_GROUP_ID
        if(strlen(arg[1]) <= MAX_GROUP_ID){
            *groupName = (char *) calloc(strlen(arg[1])+1,sizeof(char)); //!!
            strcpy(*groupName,arg[1]);

            if(strcmp(CREATE_CMD,arg[0]) == 0){
                return CREATE_DES;
            } else if(strcmp(DELETE_CMD,arg[0]) == 0){
                return DELETE_DES;
            } else if(strcmp(GROUP_CMD,arg[0]) == 0){
                return GROUP_DES;
            }
        } else{
            printf("Group name too large\n");
            return 0;
        }
    }
    // if too many are given since all commands considered only take two args
    else {
        printf("Too many arguments\n");
        return 0;
    }

    printf("Invalid command\n");
    return 0;
}