#include "KVSLocalServer-data.h" // Include header
#include "KVSLocalServer-client.h"

// ---------- Global variables ----------
GROUP * groups = NULL; // Pointer to the first element of the linked list of groups 

// ---------- Data management functions ----------
int groupAdd(char * groupId){
    // 1. Just allocate memory 
    // Allocate group
    GROUP * newGroup = (GROUP *) malloc(sizeof(GROUP));
    // Catch allocation error
    if(newGroup == NULL){
        return GROUP_ALLOC_ERROR;
    }
    // Set prox to NULL
    newGroup->prox = NULL;
    // Allocate memory for group id
    newGroup->id = (char * ) malloc(sizeof(char)*(strlen(groupId)+1));
    // Catch allocation error
    if(newGroup->id == NULL){
        free(newGroup); // Free allocated block
        return GROUP_ALLOC_ERROR;
    }
    strcpy(newGroup->id,groupId); // Set group id string
    newGroup->numberEntries = 0; // Init number of entries

    // 2. Check if group already exists
    // (note that only one thread can create groups) thus one can readlock and readunlock and 
    // guaratee that afterwards the group does not exists 
    // [READ LOCK groups]
    GROUP * searchPointer = groups;
    // Iterate through the groups until the last, which does not point to other GROUP block
    while(searchPointer != NULL){
        // Check if group already exists
        if(strcmp(searchPointer->id,groupId)==0){
                // [READ UNLOCK groups]
                free(newGroup->id);
                free(newGroup);
                return GROUP_ALREADY_EXISTS;
        }
        searchPointer = searchPointer->prox;
    }
    // [READ UNLOCK groups]

    // 3. Get group secret and communicate with auth server
    char * secret; // Define access pointer to generated secret
    
    // Catch errors on secret generation and broadcast to KVS server
    switch(authCreateGroup(groupId,&secret)){ 
        case AUTH_OK: // Successfull 
            break;
        case AUTH_ALLOC_ERROR: // Alocation error
            free(newGroup->id);
            free(newGroup);
            return GROUP_ALLOC_ERROR;
        // Communication error with auth server (secret has already been freed)
        case AUTH_COM_ERROR:
            free(newGroup->id);
            free(newGroup);
            return GROUP_AUTH_COM_ERROR;
        // Group already exists in server
        // Occurs if group is created, KVS server shuts down in an uncontrolled manner, 
        // KVSserver reboots, and then attempts to create a group with the same name
        case AUTH_GROUP_ALREADY_EXISTS: 
            free(newGroup->id);
            free(newGroup);
            return GROUP_LOSS_SYNCH;
    }
    printf("Created group -> Group id: %s | Secret: %s \n",groupId, secret);
    free(secret);
    // 4. Add group to list
    // There is only one thread allowed to add groups thus there is no nedd for write lock 
    // [READ LOCK groups]
    // If there are not any groups in the list
    searchPointer = groups;
    if(groups == NULL){
        // Allocate group in the beginning of the list
        groups = newGroup;
    }else{
        while(searchPointer->prox != NULL){
            searchPointer = searchPointer->prox;
        }
        // Add group
        searchPointer->prox = newGroup;
    }
    // [READ UNLOCK groups]
    return GROUP_OK;
}

int groupDelete(char * groupId){
    // ---------- Rearrange list ----------
    // Allocate pointer to group list
    GROUP * prev = NULL;
    // [WRITE LOCK groups]
    GROUP * searchPointer = groups;
    // Find group
    while(1){
        // If end of the list is reached without finding the group
        if(searchPointer == NULL){
            // [WRITE UNLOCK groups]
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
    // [WRITE UNLOCK groups]

    // ---------- Handle deletion of group block ----------
    // Block is no longer accessible from the list
    // But it is accessible from clients
    // Remove access from clients
    clientDeleteAccessGroup(searchPointer);
    entriesDelete(searchPointer); // delete entries of group
    free(searchPointer->id); // delete group id of group
    free(searchPointer); // delete group block
        
    printf("Deleted group -> Group id: %s\n",groupId);
    return GROUP_OK;
}

// Delete all entries of a given group
// No need to use thread safe code, because it is ensured that one this function runs
// it is not possible to acess the group block or its entries
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
    // Allocate number of entries beforehand, so that unlock is done sooner
    int numberOfEntries;
    // [READ LOCK groups]
    // Allocate pinter to group list
    GROUP * searchPointer = groups;
    // Iterate until the desired group is found
    while(1){
        // If end of the list has been reached
        if(searchPointer == NULL){
            // [READ UNLOCK groups]
            return GROUP_DSNT_EXIST;
        }
        // If groupId is found
        if(strcmp(searchPointer->id,groupId)==0){
            numberOfEntries = searchPointer->numberEntries;
            break;
        }
        searchPointer = searchPointer->prox;
    }
    // [READ UNLOCK groups]
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
        case AUTH_GROUP_DSN_EXIST:
            return GROUP_LOSS_SYNCH;
    }
    // Print group info
    printf("Group id: %s | Secret: %s | Number of key-value pairs: %d\n",groupId,secret,numberOfEntries);
    free(secret); // Free allocated memory
    return GROUP_OK;
}

void groupClear(){
    // Allocate pointer to group list
    GROUP * prev = NULL;
    // [WRITE LOCK groups]
    GROUP * searchPointer = groups;
    // Iterate through all groups
    while(searchPointer != NULL){
        // Check next element on the list
        prev = searchPointer;
        searchPointer = searchPointer->prox;
        // No need to delete client acess to group because the handler thread has 
        // already been joined when this function runs
        entriesDelete(prev); // delete entries of group
        free(prev->id); // delete group id of group
        free(prev); // delete group block
    }
    // [WRITE UNLOCK groups]
    printf("Cleared all groups.\n");
}