#include "KVSLocalServer-base.h"
#include "KVSLocalServer-data.h" // Include header
#include "KVSLocalServer-auth.h"

// ---------- Global variables ----------
GROUP * groups = NULL; // Pointer to the first element of the linked list of groups 
pthread_rwlock_t groups_rwlock = PTHREAD_RWLOCK_INITIALIZER;

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
    newGroup->entries = NULL;
    pthread_rwlock_init(&newGroup->entries_rwlock, NULL);
    // Allocate memory for group id
    newGroup->id = (char * ) malloc(sizeof(char)*(strlen(groupId)+1));
    // Catch allocation error
    if(newGroup->id == NULL){
        pthread_rwlock_destroy(&newGroup->entries_rwlock);
        free(newGroup); // Free allocated block
        return GROUP_ALLOC_ERROR;
    }
    strcpy(newGroup->id,groupId); // Set group id string
    newGroup->numberEntries = 0; // Init number of entries

    // 2. Check if group already exists
    // (note that only one thread can create groups) thus one can readlock and readunlock and 
    // guaratee that afterwards the group does not exists 
    // [READ LOCK groups]
    pthread_rwlock_rdlock(&groups_rwlock);
    GROUP * searchPointer = groups;
    // Iterate through the groups until the last, which does not point to other GROUP block
    while(searchPointer != NULL){
        // Check if group already exists
        if(strcmp(searchPointer->id,groupId)==0){
                pthread_rwlock_unlock(&groups_rwlock);
                // [READ UNLOCK groups]
                pthread_rwlock_destroy(&newGroup->entries_rwlock);
                free(newGroup->id);
                free(newGroup);
                return GROUP_ALREADY_EXISTS;
        }
        searchPointer = searchPointer->prox;
    }
    pthread_rwlock_unlock(&groups_rwlock);
    // [READ UNLOCK groups]
    // 3. Get group secret and communicate with auth server
    // Define access pointer to generated secret
    #ifdef DEBUG_SMALL_SECRET_LEN
    // Allicate memory for secrete 
    char * secret = (char *) malloc(DEBUG_SMALL_SECRET_LEN); 
    if(secret == NULL){ // Cacth allocation error
        pthread_rwlock_destroy(&newGroup->entries_rwlock);
        free(newGroup->id);
        free(newGroup);
        return GROUP_ALLOC_ERROR;
    }
    for(int i= 0; i< DEBUG_SMALL_SECRET_LEN-1; i++){
        *(secre +i) = (char) 33+(rand()%94); // generate numbers 
    }
    *(secret+DEBUG_SMALL_SECRET_LEN-1) = '\0';
    #else
    // Allicate memory for secrete 
    char * secret = (char *) malloc(MAX_SECRET_LEN); 
    if(secret == NULL){ // Cacth allocation error 
        pthread_rwlock_destroy(&newGroup->entries_rwlock);
        free(newGroup->id);
        free(newGroup);
        return GROUP_ALLOC_ERROR;
    }
    for(int i= 0; i< MAX_SECRET_LEN-1; i++){
        *(secret +i) = (char) 33+(rand()%94); // generate numbers 
    }
    *(secrete+MAX_SECRET_LEN-1) = '\0';
    #endif 
    
    // Create group on authentication server 
    int status = authCreateGroup(groupId,secret);
    // Catch errors on secret generation and broadcast to KVS server
    // Communication error with auth server
    if (status == AUTH_IMPOSSIBLE_SERVER || status == AUTH_SENDING || status == AUTH_RECEIVING || status == AUTH_INVALID){
        pthread_rwlock_destroy(&newGroup->entries_rwlock);
        free(newGroup->id);
        free(newGroup);
        free(secret);
        return GROUP_AUTH_COM_ERROR;
    }
    switch(status){ 
        case AUTH_OK: // Successfull 
            break;
        case AUTH_ALLOC_ERROR: // Alocation error
            pthread_rwlock_destroy(&newGroup->entries_rwlock);
            free(newGroup->id);
            free(newGroup);
            free(secret);
            return GROUP_ALLOC_ERROR;
        // Group already exists in server
        // Occurs if group is created, KVS server shuts down in an uncontrolled manner, 
        // KVSserver reboots, and then attempts to create a group with the same name
        // OR
        // It is a group of other KVS Local Server
        case AUTH_GROUP_ALREADY_EXISTS: 
            pthread_rwlock_destroy(&newGroup->entries_rwlock);
            free(newGroup->id);
            free(newGroup);
            free(secret);
            return GROUP_LOSS_SYNCH;
    }
    printf("Created group -> Group id: %s | Secret: %s \n",groupId, secret);
    free(secret);
    // 4. Add group to list
    // There is only one thread allowed to add groups thus there is no nedd for write lock 
    // [READ LOCK groups]
    pthread_rwlock_rdlock(&groups_rwlock);
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
    pthread_rwlock_unlock(&groups_rwlock);
    // [READ UNLOCK groups]
    return GROUP_OK;
}

int groupDelete(char * groupId){
    // Delete group from authentication server 
    int status = authDeleteGroup(groupId);
    // Catch errors on secret generation and broadcast to KVS server
    // Communication error with auth server
    if (status == AUTH_IMPOSSIBLE_SERVER || status == AUTH_SENDING || status == AUTH_RECEIVING || status == AUTH_INVALID){
        return GROUP_AUTH_COM_ERROR;
    }
    if(status == GROUP_DSNT_EXIST){
        return GROUP_DSNT_EXIST;
    }

    // ---------- Rearrange list ----------
    // Allocate pointer to group list
    GROUP * prev = NULL;
    // [WRITE LOCK groups]
    pthread_rwlock_wrlock(&groups_rwlock);
    GROUP * searchPointer = groups;
    // Find group
    while(1){
        // If end of the list is reached without finding the group
        if(searchPointer == NULL){
            pthread_rwlock_unlock(&groups_rwlock);
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
    pthread_rwlock_unlock(&groups_rwlock);
    // [WRITE UNLOCK groups]

    // ---------- Handle deletion of group block ----------
    // Block is no longer accessible from the list
    // But it is accessible from clients
    // Remove access from clients
    clientDeleteAccessGroup(searchPointer);
    pthread_rwlock_destroy(&searchPointer->entries_rwlock); // destroy rw lock
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
    ENTRY * prev = NULL;
    // Set searchPointer to initial pointer of the list
    ENTRY * searchPointer = group->entries;
    // Iterate through all blocks 
    while(searchPointer != NULL){
        prev = searchPointer;
        searchPointer = searchPointer->prox;
        // Free memory
        free(prev->key);
        free(prev->value);
        free(prev); // Free memory
    }
}
  
int groupShow(char * groupId){
    // Allocate number of entries beforehand, so that unlock is done sooner
    int numberOfEntries;
    // [READ LOCK groups]
    pthread_rwlock_rdlock(&groups_rwlock);
    // Allocate pinter to group list
    GROUP * searchPointer = groups;
    // Iterate until the desired group is found
    while(1){
        // If end of the list has been reached
        if(searchPointer == NULL){
            pthread_rwlock_unlock(&groups_rwlock);
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
    pthread_rwlock_unlock(&groups_rwlock);
    // [READ UNLOCK groups]
    char * secret = (char *) malloc(MAX_SECRET_LEN); // Allocate pointer to secret
    if (secret == NULL){
        fprintf(stderr,"Error allocating memory to shown group.\n");
        return GROUP_ALLOC_ERROR;
    }
    // Catch errors on secret query to auth server
    switch(authGetSecret(groupId,secret)){
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

int groupAddEntry(CLIENT * client, char * key, char * value){
    //0. Allocate Entry block just in case  (to avoid doing it in the write lock)
    ENTRY * newEntry = (ENTRY * ) malloc(sizeof(ENTRY));
    if(newEntry == NULL){
        // Free memory on error
        free(key);
        free(value);
        return STATUS_ALLOC_ERROR;
    }
    newEntry->key = key;
    newEntry->value = value;
    newEntry->prox = NULL;
    // Allocate auxiliar pointer
    char * aux;

    // [MUTEX AuthGroup]
    pthread_mutex_lock(&client->authGroup_mtx);
    // 1. Check if the authorized group address is valid 
    if(client->connectivityStatus != CONN_STATUS_CONNECTED || client->authGroup == NULL){
        pthread_mutex_unlock(&client->authGroup_mtx);
        // [READ UNLOCK AuthClient]
        // Free memory on error
        free(key);
        free(value);
        free(newEntry);
        return STATUS_ACCSS_DENIED;
    }
    // 2. Add value
    ENTRY * prev = NULL;
    // [WRITE LOCK ENTRIES] 
    pthread_rwlock_wrlock(&client->authGroup->entries_rwlock);
    ENTRY * searchEntry = client->authGroup->entries;
    while(1){
        // If end of the list is reached
        if (searchEntry == NULL){
            if(prev == NULL){
                client->authGroup->entries = newEntry;
            }else{
                prev->prox = newEntry;
            }
            client->authGroup->numberEntries++;
            pthread_rwlock_unlock(&client->authGroup->entries_rwlock);
            // [WRITE UNLOCK ENTRIES]
            pthread_mutex_unlock(&client->authGroup_mtx);
            // [READ UNLOCK AuthClient]
            break;
        }
        // If key is found
        if(strcmp(searchEntry->key,key)==0){
            aux = searchEntry->value;
            searchEntry->value = value;
            // [WRITE UNLOCK ENTRIES]
            pthread_rwlock_unlock(&client->authGroup->entries_rwlock);
            // [READ UNLOCK AuthClient]
            pthread_mutex_unlock(&client->authGroup_mtx);
            free(aux); // free previous value
            free(key); // free received key
            free(newEntry); // free entry, which was allocated just in case
            break;
        }
        prev = searchEntry;
        searchEntry = searchEntry->prox;
    }
    return STATUS_OK;   
}

int groupReadEntry(CLIENT * client, char * key, char ** val, uint64_t * valLen){
    // [READ LOCK AuthClient]
    pthread_mutex_lock(&client->authGroup_mtx);
    // 1. Check if the authorized group address is valid 
    if(client->connectivityStatus != CONN_STATUS_CONNECTED || client->authGroup == NULL){
        pthread_mutex_unlock(&client->authGroup_mtx);
        // [READ UNLOCK AuthClient]
        // Free memory on error
        return STATUS_ACCSS_DENIED;
    }
    // 2. Seach key
    ENTRY * prev = NULL;
    // [READ LOCK ENTRIES] 
    pthread_rwlock_rdlock(&client->authGroup->entries_rwlock);
    ENTRY * searchEntry = client->authGroup->entries;
    while(1){
        // If end of the list is reached
        if (searchEntry == NULL){
            pthread_rwlock_unlock(&client->authGroup->entries_rwlock);
            // [READ UNLOCK ENTRIES]
            pthread_mutex_unlock(&client->authGroup_mtx);
            // [READ UNLOCK AuthClient]
            return STATUS_GROUP_DSN_EXIST;
        }
        // If key is found
        if(strcmp(searchEntry->key,key)==0){
            *valLen = strlen(searchEntry->value)+1;
            *val = (char *) malloc(*valLen);
            if(*val == NULL){
                pthread_rwlock_unlock(&client->authGroup->entries_rwlock);
                // [WRITE UNLOCK ENTRIES]
                pthread_mutex_unlock(&client->authGroup_mtx);
                // [READ UNLOCK AuthClient]
                return STATUS_ALLOC_ERROR;
            }
            strcpy(*val,searchEntry->value);
            pthread_rwlock_unlock(&client->authGroup->entries_rwlock);
            // [WRITE UNLOCK ENTRIES]
            pthread_mutex_unlock(&client->authGroup_mtx);
            // [READ UNLOCK AuthClient]
            break;
        }
        prev = searchEntry;
        searchEntry = searchEntry->prox;
    }
    return STATUS_OK;
}

int groupDeleteEntry(struct clientStruct * client, char * key){
    // [READ LOCK AuthClient]
    pthread_mutex_lock(&client->authGroup_mtx);
    // 1. Check if the authorized group address is valid 
    if(client->connectivityStatus != CONN_STATUS_CONNECTED || client->authGroup == NULL){
        pthread_mutex_unlock(&client->authGroup_mtx);
        // [READ UNLOCK AuthClient]
        return STATUS_ACCSS_DENIED;
    }
    // 2. Search value
    ENTRY * prev = NULL;
    // [WRITE LOCK ENTRIES] 
    pthread_rwlock_wrlock(&client->authGroup->entries_rwlock);
    ENTRY * searchEntry = client->authGroup->entries;
    while(1){
        // If end of the list is reached
        if (searchEntry == NULL){
            pthread_rwlock_unlock(&client->authGroup->entries_rwlock);
            // [WRITE UNLOCK ENTRIES]
            pthread_mutex_unlock(&client->authGroup_mtx);
            // [READ UNLOCK AuthClient]
            return STATUS_GROUP_DSN_EXIST;
        }
        // If key is found
        if(strcmp(searchEntry->key,key)==0){
            if(prev == NULL){
                client->authGroup->entries = searchEntry->prox;
            }else{
                prev->prox = searchEntry->prox;
            }
            client->authGroup->numberEntries--;
            pthread_rwlock_unlock(&client->authGroup->entries_rwlock);
            // [WRITE UNLOCK ENTRIES]
            pthread_mutex_unlock(&client->authGroup_mtx);
            // [READ UNLOCK AuthClient]
            break;
        }
        prev = searchEntry;
        searchEntry = searchEntry->prox;
    }
    // Free memory
    free(searchEntry->key);
    free(searchEntry->value);
    free(searchEntry);

    return STATUS_OK;
}

void groupClear(){
    // Allocate pointer to group list
    GROUP * prev = NULL;
    // [WRITE LOCK groups]
    pthread_rwlock_wrlock(&groups_rwlock);
    GROUP * searchPointer = groups;
    // Iterate through all groups
    while(searchPointer != NULL){
        // Check next element on the list
        prev = searchPointer;
        searchPointer = searchPointer->prox;
        // No need to delete client acess to group because the handler thread has 
        // already been joined when this function runs
        entriesDelete(prev); // delete entries of group
        pthread_rwlock_destroy(&prev->entries_rwlock);
        free(prev->id); // delete group id of group
        free(prev); // delete group block
    }
    pthread_rwlock_unlock(&groups_rwlock);
    printf("Cleared all groups.\n");
}