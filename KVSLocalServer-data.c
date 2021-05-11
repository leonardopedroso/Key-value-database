#include "KVSLocalServer-data.h" // Include header

// ---------- Global variables ----------
GROUP * groups = NULL; // Pointer to the first element of the linked list of groups 

// ---------- Data management functions ----------
int groupAdd(char * groupId){
    // Allocate pointers to group list
    GROUP * prev = NULL;
    GROUP * searchPointer = groups;
    // If there are not any groups in the list
    if(groups == NULL){
        // Allocate group in the beginning of the list
        groups = (GROUP *) malloc(sizeof(GROUP));
        // Catch allocation error
        if(groups == NULL){
            return GROUP_ALLOC_ERROR;
        }
        // Set pointer of allocated group 
        searchPointer = groups;
    }else{
        // If first block has the same id
        if(searchPointer->prox == NULL && strcmp(searchPointer->id,groupId)==0){
                return GROUP_ALREADY_EXISTS;
        }
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
        // Set pointer of allocated group and previous element in the list
        prev = searchPointer;
        searchPointer = searchPointer->prox;
    }
    // Allocate memory for group id
    searchPointer->id = (char * ) malloc(sizeof(char)*(strlen(groupId)+1));
    // Catch allocation error
    if(searchPointer->id == NULL){
        // If element is not the first delete reference to block
        if(prev != NULL){
            prev->prox = NULL;
        }else{ // delete reference if it is the first
            groups = NULL;
        }
        free(searchPointer); // Free allocated block
        return GROUP_ALLOC_ERROR;
    }
    strcpy(searchPointer->id,groupId); // Set group id string
    searchPointer->numberEntries = 0; // Init number of entries
    char * secret; // Define access pointer to generated secret
    int flagAuthFatalError; // Allocate error flag
    // Catch errors on secret generation and broadcast to KVS server
    switch(authCreateGroup(groupId,&secret)){ 
        case AUTH_OK: // Successfull 
            flagAuthFatalError = GROUP_OK;
            break;
        case AUTH_ALLOC_ERROR: // Alocation error
            flagAuthFatalError = GROUP_ALLOC_ERROR;
            break;
        // Communication error with auth server (secret has already been freed)
        case AUTH_COM_ERROR:
            flagAuthFatalError = GROUP_AUTH_COM_ERROR;
            break;
        // Group already exists in server
        // Occurs if group is created, KVS server shuts down in an uncontrolled manner, 
        // KVSserver reboots, and then attempts to create a group with the same name
        case AUTH_GROUP_ALREADY_EXISTS: 
            flagAuthFatalError = GROUP_LOSS_SYNCH;
            break;
    }
    // If no error was caught
    if(flagAuthFatalError == GROUP_OK){
        printf("Created group -> Group id: %s | Secret: %s \n",groupId, secret);
        free(secret); // free generated secret
        return GROUP_OK;
    }else{ // On error free newly created group and id string
        // If element is not the first delete reference to block
        if(prev != NULL){
            prev->prox = NULL;
        }else{ // delete reference if it is the first
            groups = NULL;
        }
        free(searchPointer->id); // Free allocated string for group id
        free(searchPointer); // Free allocated block
        return flagAuthFatalError;
    }
}

int groupDelete(char * groupId){
    // Allocate pointer to group list
    GROUP * prev = NULL;
    GROUP * searchPointer = groups;
    // Find group
    while(1){
        // If end of the list is reached without finding the group
        if(searchPointer == NULL){
            return GROUP_DSNT_EXIST;
        }
        // Check until group is found
        if(strcmp(searchPointer->id,groupId) == 0){
            break;
        }
        // Check next element on the list
        prev = searchPointer;
        searchPointer = searchPointer->prox;
    }
    // Connect previous element on the list to the next block
    if (prev == NULL){ // Then block to delete is the first element of the list
        groups = searchPointer->prox;
    }else{
        prev->prox = searchPointer->prox;
    }
    //[MUTEX IN REGION ENTRIES]
    entriesDelete(searchPointer); // delete entries of group
    free(searchPointer->id); // delete group id of group
    free(searchPointer); // delete group block
    //[MUTEX OUT REGION ENTRIES]
    printf("Deleted group -> Group id: %s\n",groupId);
    return GROUP_OK;
}

// Delete all entries of a given group
void entriesDelete(GROUP * group){
    // Allocate vectors to entry list 
    ENTRY * prev;
    // Set searchPointer to initial pointer of the list
    ENTRY * searchPointer = group->entries;
    // Iterate through all blocks 
    while(searchPointer != NULL){
        prev = searchPointer;
        searchPointer = searchPointer->prox;
        free(prev); // Free memory
    }
}
  
int groupShow(char * groupId){
    // Allocate pinter to group list
    GROUP * searchPointer = groups;
    // Iterate until the desired group is found
    while(1){
        // If end of the list has been reached
        if(searchPointer == NULL){
            return GROUP_DSNT_EXIST;
        }
        // If groupId is found
        if(strcmp(searchPointer->id,groupId)==0){
            break;
        }
        searchPointer = searchPointer->prox;
    }
    char * secret; // Allocate pointer to secret
    // Catch errors on secret query to auth server
    switch(authGetSecret(groupId,&secret)){
        case AUTH_OK: // Success
            break;
        // Communication error with auth server (secret has already been freed)
        case AUTH_COM_ERROR:
            return GROUP_AUTH_COM_ERROR;
        // Group already exists in server
        // Occurs if group is created, KVS auth server shuts down in an uncontrolled manner, 
        // KVS auth server reboots and lost group information
        case AUTH_GOUP_DSN_EXIST:
            return GROUP_LOSS_SYNCH;
    }
    // Print group info
    printf("Group id: %s | Secret: %s | Number of key-value pairs: %d\n",groupId,secret,searchPointer->numberEntries);
    free(secret); // Free allocated memory
    return GROUP_OK;
}

void groupClear(){
    // Allocate pointer to group list
    GROUP * prev = NULL;
    GROUP * searchPointer = groups;
    // Iterate through all groups
    while(searchPointer != NULL){
        // Check next element on the list
        prev = searchPointer;
        searchPointer = searchPointer->prox;
        //[MUTEX IN REGION ENTRIES]
        entriesDelete(prev); // delete entries of group
        free(prev->id); // delete group id of group
        free(prev); // delete group block
        //[MUTEX OUT REGION ENTRIES]
    }
    printf("Cleared all groups.\n");
}