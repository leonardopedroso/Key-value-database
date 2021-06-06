#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "KVS-lib.h"

void printChanged(char * key){
    printf("I am a callback function to tell you that %s changed!\n",key);
    fflush(stdout);
}


int main(){
    int aux;
    char a[10] = "ab";
    char b[10] = "zeleo";
    char key1[10] = "key1";
    char val1[10] = "val1";
    char valNew[19] = "valNew";

    if((aux = establish_connection(a,b))!=0){
        printf("Establish connection error %d.\n",aux);
        exit(0);
    }

    // creates a key
    printf("put value %d\n",put_value(key1,val1));
    // to make sure every one registers a callback
    getchar();
    // triggers the callback
    printf("put value %d\n",put_value(key1,valNew));

    printf("Close connection %d.\n",close_connection());
    
    exit(0);
}