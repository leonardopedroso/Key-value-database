#include <stdlib.h>
#include <stdio.h>
#include "KVS-lib.h"


int main(){

    printf("PID: %d\n",getpid());

    char a[10] = "ab";
    char b[10] = "cd";
    switch (establish_connection(a,b)){
        case SUCCESS:
            printf("Established connection.\n");
            break;
        default:
            printf("Error establishing connection.\n");
            break;
    }

    char * out;

    char key1[10] = "key1";
    char val1[10] = "val1";
    if(put_value(key1,val1)!=0){
        printf("Put value error %lu.\n", strlen(key1)+1);
    }
    printf("Test set 1 | Write | Key: %s | Value: %s\n",key1,val1);
    if(get_value(key1,&out)!=0){
        printf("GEt value error.\n");
    }
    printf("Test set 1 | Read | Key: %s | Value: %s\n",key1,out);
    free(out);

    if(delete_value(key1)!=0){
        printf("Delete value error.\n");
    }
    printf("Test set 1 | Deleye | Key: %s\n",key1);

    if(get_value(key1,&out)!=0){
        printf("GEt value error.\n");
    }else{
        printf("Test set 1 | Read | Key: %s | Value: %s\n",key1,out);
        free(out);
    }
    
    getchar();

    char key2[10] = "key2";
    char val2[10] = "val2";
    if(put_value(key2,val2)!=0){
        printf("Put value error.\n");
    }
    printf("Test set 2 | Write | Key: %s | Value: %s\n",key2,val2);
    if(get_value(key2,&out)!=0){
        printf("Get value error.\n");
    }
    printf("Test set 2 | Read | Key: %s | Value: %s\n",key2,out);
    free(out);
    

    /*char key3[10] = "key3";
    char val3[10] = "val3";
    printf("Test set 3 | Write | Key: %s | Value: %s\n",key3,val3);*/




    getchar();


    if(close_connection() == SUCCESS){
        printf("Close connection succssefull.\n");
    }else{
        printf("Close connection error.\n");
    }
    exit(0);
}