#ifndef KVS_AUTH_SERVER_DATA_H
#define KVS_AUTH_SERVER_DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "KVS-lib-MACROS.h"

typedef struct pairStruct{
    char *group;
    char *secret;

    struct pairStruct *prox;
}PAIR;

// Success
#define SUCCESS 0
// Pair already exists error
#define PAIR_ALRD_EXISTS -3
// Could not allocate memory for new pair
#define PAIR_ALLOC_ERR -4
// Pair does not exist
#define GROUP_DSNT_EXIST -5
// Empty string
#define EMPTY_STR -6
// String too big
#define TOO_BIG_STR -7
// Invalid group name
#define BAD_GROUP -8
// Invalid secret name
#define BAD_SECRET -9

// \brief verifies if a pair element (group or secret) is in accordance with
// what is expected. Assumes that there is MAX_LEN_GROUP or MAX_LEN_SECRET 
// bytes allocated to a memory block pointed to by the pointer to the string
// which is guaranteed when using as buffer when receiving a REQUEST type 
// variable. This function was designed to guarantee the security of the
// execution: if a string with no delimiter was to be ran by the following
// functions one would incur in a Segmentation Fault.
// \param pairElem string of a group or character to be checked
// \return On a good str, returns SUCCESS. On an empty one, returns EMPTY_STR.
// On a too big one, returns TOO_BIG_STR
int checkPairElem(char *pairElem);

// \brief Adds a new pair to the list. Assumes that there is MAX_LEN_GROUP or 
// MAX_LEN_SECRET bytes allocated to a memory block pointed to by the pointer
// to the group and the secret
// \param head linked list head pointer to pointer
// \param group string containing the name of the new group
// \param secret string containing the secret of the new group
// \return on success, returns SUCCESS. On already existing pair with given
// group name, returns PAIR_ALRD_EXISTS. On memory allocation memory, returns
// PAIR_ALLOC_ERR. On an invalid group or secret, returns BAD_GROUP or 
// BAD_SECRET
int addPair(PAIR **head,char *group,char *secret);

// \brief Deletes a pair from the list. Assumes that there is MAX_LEN_GROUP or 
// MAX_LEN_SECRET bytes allocated to a memory block pointed to by the pointer
// to the group and the secret
// \param head linked list head pointer to pointer
// \param group string containing the name of the group
// \return on success, returns SUCCESS. On non-existing group, returns 
// GROUP_DSNT_EXIST. On an invalid group, returns BAD_GROUP
int deletePair(PAIR **head,char *group);

// \brief Deletes all the list of pairs
// \param head linked list head pointer to pointer
// \return on success, returns SUCCESS.
int deleteAllPairs(PAIR **head);

// \brief Copies the string of the secret of a group to a given dest string
// assuming the dest string has MAX_GROUP_SECRET bytes allocated
// \param head linked list pointer
// \param group string containing the name of the group
// \param dest string to receive the string of the secret
// \return on success, returns SUCCESS. On invalid group names, returns 
// BAD_GROUP. On non-existing group, returns GROUP_DSNT_EXIST
int getSecret(PAIR *head,char *group,char* dest);

// \bried Prints information regarding all pairs
// \param head linked list head pointer
void printAllPairs(PAIR *head);

#endif