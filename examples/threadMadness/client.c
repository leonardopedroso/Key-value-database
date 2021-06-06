#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "KVS-lib.h"
#include <pthread.h>
#include <string.h>

#define NUM_THREADS 5
#define NUM_PUTS 10000

#define TEST_PUT

void printChanged(char * key){
    printf("I am a callback function to tell you that %s changed!\n",key);
}

#ifdef TEST_PUT
// One can try to do a delete ab on local server to check robustness
// We can also see if all the keys are created and that there are no memory leaks
void * threadFunc(void * arg){
    char key[40];
    char val[40];
    sprintf(val,"val_t%d",*((int *) arg));
    int status;
    for(int i = 0; i< NUM_PUTS; i++){
        sprintf(key,"key_t%d_n%d",*((int *) arg),i);
        status = put_value(key,val);
        if (status != SUCCESS){
            printf("Error %d thread %d\n",status,*((int *) arg));
            pthread_exit(NULL);
        }
    }
    pthread_exit(NULL);
}
#endif

#ifdef TEST_PUT_DEL
void * threadFunc(void * arg){
    char key[40];
    char val[40];
    sprintf(val,"val_t%d",*((int *) arg));
    if (*((int*)arg)%2 == 0){
        for(int i = 0; i< NUM_PUTS; i++){
            sprintf(key,"key_n%d",i);
            put_value(key,val);
        }
    }else{
        for(int i = NUM_PUTS-1; i >= 0; i--){
            sprintf(key,"key_n%d",i);
            delete_value(key);
        }
    }
    pthread_exit(NULL);
}
#endif

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
    free(threadNum);
    printf("All threads joined\n");

    if(close_connection() == SUCCESS){
        printf("Close connection succssefull.\n");
    }else{
        printf("Close connection error.\n");
    }

    exit(0);
}