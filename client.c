#include <stdlib.h>
#include <stdio.h>
#include "KVS-lib.h"


int main(){

    char a[10] = "ab";
    char b[10] = "cd";

    if(establish_connection(a,b) == ESTBL_CONN_SUCCESS){
        printf("Established connection.\n");
    }

    while (1)
    {
        /* code */
    }
    

    sleep(2);
    exit(0);
}