#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "KVS-lib.h"

void printChanged(char * key){
    char *value = NULL;
    get_value(key,&value);
    printf("Callback function for key %s changed to %s!\n",key,value);
    fflush(stdout);
}


int main(){
    int aux;
    char a[10] = "ab";
    char b[10] = "zeleo";
    char key1[10] = "key1";
    char val1[10] = "val1";
    char val2[10] = "val2";
    char *valGet;
    int father[2];

    // creates 4 clients
    father[0] = fork();
    father[1] = fork();

    printf("PID: %d\n",getpid());

    if((aux = establish_connection(a,b))!=0){
        printf("Establish connection error %d.\n",aux);
        exit(0);
    }

    // registers a callback on the clientCreateKey created key
    printf("register callback %d pid:%d\n",register_callback(key1,&printChanged),getpid());

    sleep(5);
    // on the father of the fathers
    if(father[0]!= 0 && father[1]!= 0){
        printf("father of all put value %d\n",put_value(key1,val2));
        exit(0);
    }
    sleep(5);

    printf("Close connection %d.\n",close_connection());
    
    exit(0);
}