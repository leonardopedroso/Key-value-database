#include "ui.h"
#include <stdlib.h>

int main(void){
    char *groupName = NULL;
    int action;
    char *secret = NULL;

    while(1){
        printMenu();

        action = getCommand(&groupName);

        switch(action){
            case CREATE_DES:
                // createGroup(groupList,secret);
                secret = (char *) calloc(10,sizeof(char));
                strcpy(secret,"Ola");
                printf("Created group %s with secret %s\n",groupName,secret);
                free(secret);
                break;
            case DELETE_DES:
                // deleteGroup(groupList);
                printf("Deleted group %s\n",groupName);
                break;
            case GROUP_DES:
                printf("Showing group %s\n",groupName);
                break;
            case APPS_DES:
                printf("Showing apps\n");
                break;
        }
    }

    free(groupName);

    return 0;
}