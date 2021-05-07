#include "ui.h"

int main(void){
    char *groupName;
    int action;

    while(1){
        printMenu();

        action = getCommand(&groupName);

        switch(action){
            case CREATE_DES:
                printf("Creating group %s\n",groupName);
                break;
            case DELETE_DES:
                printf("Deleting group %s\n",groupName);
                break;
            case GROUP_DES:
                printf("Showing group %s\n",groupName);
                break;
            case APPS_DES:
                printf("Showing apps\n");
                break;
        }
    }

    return 0;
}