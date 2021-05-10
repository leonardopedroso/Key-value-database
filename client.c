#include <stdlib.h>
#include <stdio.h>
#include "KVS-lib.h"


int main(){

    printf("PID: %d\n",getpid());

    char a[10] = "ab";
    char b[10] = "cd";
    if(establish_connection(a,b) == ESTBL_CONN_SUCCESS){
        printf("Established connection.\n");
    }

    /*if(close_connection() == CLOSE_CONN_SUCCESS){
        printf("Close connection succssefull.\n");
    }*/

    while (1){
        sleep(2);
    }
    
    exit(0);
}