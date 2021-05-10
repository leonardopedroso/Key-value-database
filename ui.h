#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "KVS-lib-MACROS.h"

// Create group command
#define CREATE_CMD "create"
// Delete group command
#define DELETE_CMD "delete"
// Show group info command
#define GROUP_CMD "group"
// Show application status command
#define APPS_CMD "apps"
// Arguments delimiter
#define ARG_DELIM " "
// Create group descriptor
#define CREATE_DES 1
// Delete group descriptor
#define DELETE_DES 2
// Show group info descriptor
#define GROUP_DES 3
// Show application status descriptor
#define APPS_DES 4

// \brief Prints the various commands of the menu of the user interface
void printMenu(void);

// Maximum input to be gotten from the keyboard
// Must be higher than the sum of MAX_GROUP_ID and the lengths of each the
// command and the delimiter
#define MAX_INPUT MAX_STR_LENGTH+20
// \brief Gets a command from the user and, in case of error, explains it
// \param groupName so that the function can communicate to the outside the 
// name of the group it received
// \return On error, returns 0 and, on success, a command descriptor
int getCommand(char *groupName);

#endif