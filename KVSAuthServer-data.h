#ifndef KVS_AUTH_SERVER_DATA_H
#define KVS_AUTH_SERVER_DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "KVS-lib-MACROS.h"

#define MAX_GROUP_LEN 256   // so that one avoids IP fragmentation when 
                            // transmitting group and secret
                            // This includes the terminator character
#define MAX_SECRET_LEN 256  // same reason as the above
                            // This also includes the terminator character

typedef struct pairStruct{
    char *group;
    char *secret;

    struct pairStruct *prox;
}PAIR;

// Pair already exists error
#define PAIR_ALRD_EXISTS -1
// Could not allocate memory for pair
#define PAIR_ALLOC_ERR -2
// Success adding pair
#define SUCCESS_ADD 0

// \brief Adds a new pair to the list. Assumes valid group and secret names
// \param head linked list head pointer to pointer
// \param group string containing the name of the new group
// \param secret string containing the secret of the new group
// \return one of the values from the above list
int addPair(PAIR **head,char *group,char *secret);

// Pair does not exist
#define GROUP_DSNT_EXIST -1
// Success deleting pair
#define SUCCESS_DEL 0

// \brief Deletes a pair from the list
// \param head linked list head pointer to pointer
// \param group string containing the name of the new group
// \return one of the values from the above list
int deletePair(PAIR **head,char *group);

// \brief Deletes all the list of pairs
// \param head linked list head pointer to pointer
void deleteAllPairs(PAIR **head);

#endif