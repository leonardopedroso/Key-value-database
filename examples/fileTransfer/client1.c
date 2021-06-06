#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "KVS-lib.h"

int main(){

    char group[10] = "asyoulik";
    char secret[10] = "zeleo";

    int status = establish_connection(group,secret);
    if(status!= SUCCESS){
        printf("Error %d establishing connection.\n",status);
        exit(0);
    }
    printf("Connection established\n");

    /*char * out;
    char key1[10] = "key1";
    char val1[10] = "val1";

    if(put_value(key1,val1)!=0){
        printf("Put value error.\n");
        exit(0);
    }
    printf("Test set 1 | Write | Key: %s | Value: %s\n",key1,val1);
    if(get_value(key1,&out)!=0){
        printf("Get value error.\n");
        exit(0);
    }
    printf("Test set 1 | Read | Key: %s | Value: %s\n",key1,out);
    free(out);

    if(register_callback(key1,&printChanged)!= SUCCESS){
        printf("Error on register callback.\n");
        exit(0);
    }
    printf("Callback registed.\n");

    getchar();

    /*if(delete_value(key1)!=0){
        printf("Delete value error.\n");
        exit(0);
    }
    printf("Test set 1 | Deleye | Key: %s\n",key1);

    char val2[10] = "val2";

    if(put_value(key1,val2)!=0){
        printf("Put value error.\n");
        exit(0);
    }

    if(get_value(key1,&out)!=0){
        printf("GEt value error.\n");
        exit(0);
    }
    printf("Test set 1 | Read | Key: %s | Value: %s\n",key1,out);
    free(out);

    
    getchar();

    char key2[10] = "key2";
    char valkey2[100] = "val21123y473t8473t427mlkjv";
    if(put_value(key2,valkey2)!=0){
        printf("Put value error.\n");
        exit(0);
    }
    printf("Test set 2 | Write | Key: %s | Value: %s\n",key2,valkey2);
    if(get_value(key2,&out)!=0){
        printf("Get value error.\n");
        exit(0);
    }
    printf("Test set 2 | Read | Key: %s | Value: %s\n",key2,out);
    free(out);
    

    /*char key3[10] = "key3";
    char val3[10] = "val3";
    printf("Test set 3 | Write | Key: %s | Value: %s\n",key3,val3);
    getchar();

    if(close_connection() == SUCCESS){
        printf("Close connection succssefull.\n");
    }else{
        printf("Close connection error.\n");
    }*/
    exit(0);
}