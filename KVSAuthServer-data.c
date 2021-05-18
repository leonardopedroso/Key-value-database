#include "KVSAuthServer-data.h"

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

int addPair(PAIR **head,char *group,char *secret){
    // initializes the pointers with a useful value
    PAIR *ptr = *head;
    PAIR *prev = *head;

    // gets the pointer to the last element of the list checking the group name
    while(ptr!=NULL){
        if(strcmp(ptr->group,group)==0){
            return PAIR_ALRD_EXISTS;
        }
        prev = ptr;
        ptr = ptr->prox;
    }

    // allocates memory for the new pair
    ptr = (PAIR*) calloc(1,sizeof(PAIR));
    if(ptr==NULL){
        return PAIR_ALLOC_ERR;
    }
    // allocates the elements of the pair
    ptr->group = (char*) calloc(strlen(group)+1,sizeof(char));
    ptr->secret = (char*) calloc(strlen(secret)+1,sizeof(char));
    // if one of them is badly allocated
    if(ptr->group == NULL || ptr->secret == NULL){
        // frees both. the free function on a NULL pointer does not lead to
        // errors, just does not do anything
        free(ptr->group);
        free(ptr->secret);
        // here is a difference between the two cases. so that the information
        free(ptr);
        return PAIR_ALLOC_ERR;
    }

    // locks mutex for changes in common data
    if (pthread_mutex_lock(&mtx) != 0){
        return LOCK_MTX;
    }

    // everything being well allocated, fills in the information
    strcpy(ptr->group,group);
    strcpy(ptr->secret,secret);
    ptr->prox = NULL;
    
    // assigns the new pair to a place in the list
    if(prev!=NULL){
        prev->prox = ptr;
    }else{
        *head = ptr;
    }

    if (pthread_mutex_unlock(&mtx) != 0){
        return UNLOCK_MTX;
    }

    return SUCCESS;
}

int deletePair(PAIR **head,char *group){
    PAIR * prev = NULL;
    PAIR * ptr = *head;
    
    // Find group
    while(1){
        // If end of the list is reached without finding the group
        if(ptr == NULL){
            return GROUP_DSNT_EXIST;
        }
        // Check until group is found
        if(strcmp(ptr->group,group) == 0){
            break;
        }
        // Check next element on the list
        prev = ptr;
        ptr = ptr->prox;
    }
    
    // locks mutex for changes in common data
    if (pthread_mutex_lock(&mtx) != 0){
        return LOCK_MTX;
    }
    
    // Connect previous element on the list to the next block
    if (prev == NULL){ // Then block to delete is the first element of the list
        *head = ptr->prox;
    }else{
        prev->prox = ptr->prox;
    }

    free(ptr->group);
    free(ptr->secret);
    free(ptr);
    
    if (pthread_mutex_unlock(&mtx) != 0){
        return UNLOCK_MTX;
    }
    
    return SUCCESS;
}

int deleteAllPairs(PAIR **head){
    PAIR *prev = NULL;
    PAIR *ptr = NULL;

    // if the list is already empty
    if(*head == NULL){
        return EMPTY;
    }

    // if not starts at the head
    prev = *head;
    ptr = (*head)->prox;

    // locks mutex for changes in common data
    if (pthread_mutex_lock(&mtx) != 0){
        return LOCK_MTX;
    }

    // and while there is more than one element in the list
    while(ptr != NULL){
        // surfs to the last element
        while(ptr->prox != NULL){
            prev = ptr;
            ptr = ptr->prox;
        }

        // deletes the last element and unlinks it
        free(ptr->group);
        free(ptr->secret);
        free(ptr);
        prev->prox = NULL;

        // comes back to the beginning of the list
        ptr = (*head)->prox;
        prev = *head;
    }

    // when there is only one element, deletes it and nulls the head
    free((*head)->group);
    free((*head)->secret);
    free((*head));
    *head = NULL;

    // locks mutex for changes in common data
    if (pthread_mutex_unlock(&mtx) != 0){
        return LOCK_MTX;
    }

    return SUCCESS;
}