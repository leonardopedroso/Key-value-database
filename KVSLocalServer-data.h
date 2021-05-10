#ifndef KVS_LOCAL_SERVER_DATA_H
#define KVS_LOCAL_SERVER_DATA_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h> // to manipulate strings

// Struct to hold key-value pairs
typedef struct entryStruct{
    char * key; //with malloc instead of char key[MAX_STR_LENGTH];
    char * value; //with malloc instead of char value[MAX_STR_LENGTH];

    struct entryStruct * prox;
}ENTRY;

// Struct to hold 
typedef struct groupStruct{
    char * id; //with malloc instead of char id[MAX_STR_LENGTH];
    ENTRY * entries;

    struct groupStruct * prox;
}GROUP;

// ---------- Data management prototypes ----------
#define GROUP_OK 0
int groupAdd(char * group);
int groupDelete(char * group);
int groupShow(char * group);

#endif