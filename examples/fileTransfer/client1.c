#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "KVS-lib.h"

#define INPUT_FILE "asyoulik.txt"
//#define INPUT_FILE "church.jpg"
//#define INPUT_FILE "marbles.BMP"

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

    // ---------- Open shakespeare play
    FILE * fd = fopen(INPUT_FILE, "r");
    fseek(fd, 0L, SEEK_END);
    uint64_t sz = ftell(fd);
    fclose(fd);
    fd = fopen(INPUT_FILE, "r");
    char * data = (char *) malloc(sz); 
    if(data == NULL){
        printf("Allocation error.\n");
        exit(0);
    }   
    fread(&data[0], 1, sz+100, fd);
    fclose(fd);
    printf("Shakespeare play loaded.\n");

    // --------- Put value
    char key[10] = "asyoulik";
    status = put_value(key,data);
    if(status != SUCCESS){
        printf("Put value error %d.\n", status);
        exit(0);
    }
    printf("Shakespeare play uploaded to KVS local server (%llu bytes).\n",sz);

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