#include "KVSLocalServer-data.h" // Include header

// ---------- Global variables ----------
GROUP * groups = NULL; // Pointer to the first element of the linked list of groups 

// ---------- Data management functions ----------
int groupAdd(char * groupId){

    printf("1\n");
    // Allocate pointers to group list
    GROUP * prev;
    GROUP * searchPointer = groups;
    printf("2\n");
    // If there are not any groups in the list
    if(groups == NULL){
        printf("3 NULL\n");
        // Allocate group in the beginning of the list
        groups = (GROUP *) malloc(sizeof(GROUP));
        
        // Catch allocation error
        if(groups == NULL){
            return GROUP_ALLOC_ERROR;
        }
        printf("4\n");
        searchPointer = groups;
    }else{
        printf("3 N NULL\n");
        // Iterate through the groups until the last, which does not point to other GROUP block
        while(searchPointer->prox != NULL){
            // Check if group already exists
            if(strcmp(searchPointer->id,groupId)==0){
                return GROUP_ALREADY_EXISTS;
            }
            searchPointer = searchPointer->prox;
        }
        // Allocate group
        searchPointer->prox = (GROUP *) malloc(sizeof(GROUP));
        // Catch allocation error
        if(searchPointer->prox == NULL){
            return GROUP_ALLOC_ERROR;
        }
        printf("4\n");
        prev = searchPointer;
        searchPointer = searchPointer->prox;
    }
    
    printf("5.1\n");
    printf("%s\n",groupId);
    searchPointer->id = (char * ) malloc(sizeof(char)*(strlen(groupId)+1));
    if(searchPointer->id==NULL){
        if(prev != NULL){
            prev = NULL;
        }
        free(searchPointer);
        return GROUP_ALLOC_ERROR;
    }
    strcpy(searchPointer->id,groupId);
    printf("5.2\n");
    searchPointer->numberEntries = 0;
    printf("5.3\n");
    char * secret;
    int flagAuthFatalError = 0;
    switch(authBroadcastGroupAndSecret(groupId,&secret)){
        case AUTH_OK:
            break;
        case AUTH_ALLOC_ERROR:
            flagAuthFatalError = GROUP_ALLOC_ERROR;
            break;
        case AUTH_COM_ERROR:
            flagAuthFatalError = GROUP_AUTH_COM_ERROR;
            break;
        case AUTH_GROUP_ALREADY_EXISTS:
            flagAuthFatalError = GROUP_LOSS_SYNCH;
            break;
    }
    printf("6\n");

    if(flagAuthFatalError != 0){
        printf("Created: Group %s | Secret: %s \n",groupId, secret);
        free(secret);
        return GROUP_OK;
    }else{ // On error free newly created group
        if(prev != NULL){
            prev = NULL;
        }
        free(searchPointer);
        return flagAuthFatalError;
    }
}

int groupDelete(char * groupId){
    GROUP * prev;
    GROUP * searchPointer = groups;
    while(1){
        if(searchPointer == NULL){
            return GROUP_DSNT_EXIST;
        }
        if(strcmp(searchPointer->id,groupId)==0){
            break;
        }
        prev = searchPointer;
        searchPointer = searchPointer->prox;
        prev->prox = searchPointer->prox;
        //[MUTEX IN REGION ENTRIES]
        entryDelete(searchPointer);
        free(searchPointer->id);
        free(searchPointer);
        //[MUTEX OUT REGION ENTRIES]
    }
    return GROUP_OK;
}

void entryDelete(GROUP * group){
    ENTRY * prev;
    ENTRY * searchPointer = group->entries;
    while(searchPointer != NULL){
        prev = searchPointer;
        searchPointer = searchPointer->prox;
        free(prev);
    }
}
  
int groupShow(char * groupId){
    GROUP * searchPointer = groups;
    while(1){
        if(searchPointer == NULL){
            return GROUP_DSNT_EXIST;
        }
        if(strcmp(searchPointer->id,groupId)==0){
            break;
        }
    }
    char * secret;
    switch(authGetSecret(groupId,&secret)){
        case AUTH_OK:
            break;
        case AUTH_COM_ERROR:
            return GROUP_AUTH_COM_ERROR;
        case AUTH_GOUP_DSN_EXIST:
            return GROUP_LOSS_SYNCH;
    }
    printf("Group: %s | Secret: %s | Number of key-value pairs: %d\n",groupId,secret,searchPointer->numberEntries);
    free(secret);
    return GROUP_OK;
}