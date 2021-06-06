#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "KVS-lib.h"

void printChanged(char * key){
    printf("I am a callback function to tell you that %s changed!\n",key);
}


int main(){
    printf("PID: %d\n",getpid());

    int aux;
    char a[10] = "ab";
    char b[10] = "zeleo";
    char * out;
    char key1[10] = "key1";
    char val1[10] = "val1";

    // Test set 0 | Running without establishing connection
    // put without establishing connection
    /*if((aux = put_value(key1,val1))!=0){
        printf("Put value error %d.\n",aux);
        exit(0);
    }*/
    // get without establishing connection
    /*if((aux = get_value(key1,val1))!=0){
        printf("Put value error %d.\n",aux);
        exit(0);
    }*/
    // register callback without establishing connection
    /*if((aux = register_callback(key1,&printChanged))!=0){
        printf("Put value error %d.\n",aux);
        exit(0);
    }*/
    
    // Test set 1
    /*if((aux = establish_connection(a,b))!=0){
        printf("Establish connection error %d.\n",aux);
        exit(0);
    }*/
    /* Test set 1.1 | Registering callback without key
    if((aux = register_callback(key1,&printChanged))!=0){
        printf("Register callback error %d.\n",aux);
        fflush(stdout);
        exit(0);
    }*/
    /* Test set 1.2 | Getting non-existent value
    if((aux = get_value(key1,&printChanged))!=0){
        printf("Get value error %d.\n",aux);
        fflush(stdout);
        exit(0);
    }*/

    // Tests with established connection
    if((aux = establish_connection(a,b))!=0){
        printf("Establish connection error %d.\n",aux);
        exit(0);
    }

    // Test set 3
    /* Test set 3.1 | Using NULL keys
    char *key2 = NULL;
    char *val2 = NULL;*/
    // invalid key
    /*if((aux = put_value(key2,val1))!=0){
        printf("Put value error %d.\n",aux);
        exit(0);
    }*/
    // invalid value
    /*if((aux = put_value(key1,val2))!=0){
        printf("Put value error %d.\n",aux);
        exit(0);
    }*/
    /* Test set 3.2 | Using strings of just '\0'
    char key3[] = "\0";
    char value3[] = "\0";*/
    // invalid key
    /*if((aux = put_value(key3,val1))!=0){
        printf("Put value error %d.\n",aux);
        exit(0);
    }*/
    // invalid value
    /*if((aux = put_value(key1,val3))!=0){
        printf("Put value error %d.\n",aux);
        exit(0);
    }*/

    // Test set 4
    char *key2 = NULL;
    if((aux = get_value(key2,val1))!=0){
        printf("Get value error %d.\n",aux);
        exit(0);
    }

    if(close_connection() == SUCCESS){
        printf("Close connection succssefull.\n");
    }else{
        printf("Close connection error.\n");
    }
    exit(0);
}