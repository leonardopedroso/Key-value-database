#include "ui.h"
#include <stdlib.h>

int main(void){
    char *groupName = NULL;
    int action; // Leo: no need of this
    char *secret = NULL;

    printMenu();
 
    while(1){
        switch(getCommand(&groupName)){
            case CREATE_DES:
                // createGroup(groupList,secret);
                secret = (char *) calloc(10,sizeof(char));
                strcpy(secret,"Ola"); // TODO
                printf("Created group %s with secret %s\n\n",groupName,secret);
                free(secret);
                break;
            case DELETE_DES:
                // deleteGroup(groupList);
                printf("Deleted group %s\n\n",groupName);
                break;
            case GROUP_DES:
                printf("Showing group %s\n\n",groupName);
                break;
            case APPS_DES:
                printf("Showing apps\n\n");
                break;
            default:
                printf("\n");
                printMenu();
                break;
        }
    }

    free(groupName);

    return 0;
}