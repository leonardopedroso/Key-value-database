#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "KVS-lib.h"

#define OUTPUT_FILE "downloadClient2.txt"
//#define OUTPUT_FILE "downloadClient2.jpg"
//#define OUTPUT_FILE "downloadClient2.BMP"

int main(){

    // ---------- Establish connection with KVS Local
    char group[15] = "shakespeare";
    char secret[10] = "zeleo";
    int status = establish_connection(group,secret);
    if(status!= SUCCESS){
        printf("Error %d establishing connection.\n",status);
        exit(0);
    }
    printf("Connection established.\n");

    // --------- Get value
    char key[10] = "asyoulik";
    char * data; 
    status = get_value(key,&data);
    if(status != SUCCESS){
        printf("Get value error %d.\n", status);
        exit(0);
    }
    printf("Shakespeare downloaded from KVS local server (%llu bytes).\n", strlen(data));

    // --------- Delete value
    status = delete_value(key);
    if(status != SUCCESS){
        printf("Delete value error %d.\n", status);
        exit(0);
    }
    printf("Shakespeare play deleted from KVS local server.\n");

    // --------- Save to file
    FILE * fd = fopen(OUTPUT_FILE, "w");
    fwrite(data, 1, strlen(data), fd);
    fclose(fd);
    
    // ---------- Free memory
    free(data);

    // ---------- Close connection 
    if(close_connection() == SUCCESS){
        printf("Close connection successfull.\n");
    }else{
        printf("Close connection error.\n");
    }

    return 0;
}