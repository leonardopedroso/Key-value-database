#include <stdlib.h>
#include <stdio.h>
#include "KVS-lib.h"


int main(){

    char a[10] = "ab";
    char b[10] = "cd";

    if(establish_connection(NULL,NULL) == ESTBL_CONN_SUCCESS){
        printf("Established connection.\n");
    }

    while (1){
        /* code */
    }
    
    exit(0);
}