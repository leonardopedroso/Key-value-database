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
    char *valGet;
    int father;

    // creates 8 clients, 4 fathers and 4 sons
    fork();
    fork();
    father = fork();

    printf("PID: %d, isFather: %d\n",getpid(),father!=0);

    if((aux = establish_connection(a,b))!=0){
        printf("Establish connection error %d.\n",aux);
        exit(0);
    }

    if(father){
        printf("put value %d pid:%d isFather:%d\n",put_value(key1,val1),getpid(),father);
        printf("register callback %d pid:%d isFather:%d\n",register_callback(key1,&printChanged),getpid(),father);
        printf("get value %d pid:%d isFather:%d\n",get_value(key1,&valGet),getpid(),father);
        printf("gotten value %s pid:%d isFather:%d\n\n",valGet,getpid(),father);
    }else{
        printf("delete value %d pid:%d isFather:%d\n",delete_value(key1),getpid(),father);
    }

    printf("Close connection %d.\n",close_connection());
    
    exit(0);
}