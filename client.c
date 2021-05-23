#include <stdlib.h>
#include <stdio.h>
#include "KVS-lib.h"


int main(){

    printf("PID: %d\n",getpid());

    char a[10] = "ab";
    char b[10] = "cd";
    switch (establish_connection(a,b)){
        case SUCCESS:
            printf("Established connection.\n");
            break;
        default:
            printf("Error establishing connection.\n");
            break;
    }

    getchar();


    if(close_connection() == SUCCESS){
        printf("Close connection succssefull.\n");
    }else{
        printf("Close connection error.\n");
    }
    fflush(stdout);
    getchar();
    exit(0);
}