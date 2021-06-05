#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "KVS-lib.h"

void printChanged(char * key){
    printf("I am a callback function to tell you that %s changed!\n",key);
}


int main(){
    printf("PID: %d\n",getpid());

    char a[10] = "ab";
    char b[10] = "cd";

    int status = establish_connection(a,b);
    if(status!= SUCCESS){
        printf("Error %d establishing connection.\n",status);
        exit(0);
    }
    printf("Connection established\n");
    
    exit(0);
}