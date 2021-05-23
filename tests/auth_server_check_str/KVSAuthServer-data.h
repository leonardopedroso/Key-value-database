#ifndef KVS_AUTH_SERVER_DATA_H
#define KVS_AUTH_SERVER_DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "KVS-lib-MACROS.h"

#define MAX_GROUP_LEN 256   // so that one avoids IP fragmentation when 
                            // transmitting group and secret
                            // This includes the terminator character
#define MAX_SECRET_LEN 256  // same reason as the above
                            // This also includes the terminator character
                            // check pp. 1190 of "The Linux Programming 
                            // Interface" by Kerrisk section to see the reason 
                            // to the limits used

typedef struct pairStruct{
    char *group;
    char *secret;

    struct pairStruct *prox;
}PAIR;

// Empty list
#define EMPTY 1
// Success
#define SUCCESS 0
// Error locking mutex
#define LOCK_MTX -1
// Error unlocking mutex
#define UNLOCK_MTX -2
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

// \brief verifies if a pair element (group or secret) is in accordance with
// what is expected. Assumes that there is MAX_LEN_GROUP or MAX_LEN_SECRET bytes
// allocated to a memory block pointed to by the pointer to the string
// \param pairElem string of a group or character to be checked
// \return On a good str, returns SUCCESS. On an empty one, returns EMPTY_STR.
// On a too big one, returns TOO_BIG_STR
int checkPairElem(char *pairElem);

// \brief Adds a new pair to the list. Assumes valid group and secret names
// \param head linked list head pointer to pointer
// \param group string containing the name of the new group
// \param secret string containing the secret of the new group
// \return on success, returns SUCCESS. On errors on the mutexes, returns one 
// of the mutexes errors return values. On already existing pair with given
// group name, returns PAIR_ALRD_EXISTS. On memory allocation memory, returns
// PAIR_ALLOC_ERR
int addPair(PAIR **head,char *group,char *secret);

// \brief Deletes a pair from the list. Assumes a valid group name
// \param head linked list head pointer to pointer
// \param group string containing the name of the new group
// \return on success, returns SUCCESS. On errors on the mutexes, returns one 
// of the mutexes errors return values. On non-existing group, returns 
// GROUP_DSNT_EXIST
int deletePair(PAIR **head,char *group);

// \brief Deletes all the list of pairs
// \param head linked list head pointer to pointer
// \return on success, returns SUCCESS. On errors on the mutexes, returns one 
// of the mutexes errors return values.
int deleteAllPairs(PAIR **head);

#endif