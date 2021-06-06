#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "KVS-lib.h"
#include <pthread.h>
#include <string.h>

#define NUM_THREADS 5
#define NUM_PUTS 10000

void printChanged(char * key){
    printf("I am a callback function to tell you that %s changed!\n",key);
}

void * threadFunc(void * arg){
    char key[40];
    char val[40];
    sprintf(val,"val_t%d",*((int *) arg));
    for(int i = 0; i< NUM_PUTS; i++){
        sprintf(key,"key_t%d_n%d",*((int *) arg),i);
        put_value(key,val);
    }
    pthread_exit(NULL);
}

int main(){

    char a[10] = "ab";
    char b[10] = "zeleo";

    int status = establish_connection(a,b);
    if(status!= SUCCESS){
        printf("Error %d establishing connection.\n",status);
        exit(0);
    }
    printf("Connection established\n");

    pthread_t threads[NUM_THREADS];
    int * threadNum = (int *) malloc(sizeof(int)*NUM_THREADS);
    for(int i = 0; i < NUM_THREADS; i++){
        *(threadNum + i) = i;
        pthread_create(&threads[i],NULL,&threadFunc,threadNum+i);
    }
    printf("All threads created\n");

    for(int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i],NULL);
    }
    printf("All threads joined\n");

    if(close_connection() == SUCCESS){
        printf("Close connection succssefull.\n");
    }else{
        printf("Close connection error.\n");
    }

    exit(0);
}