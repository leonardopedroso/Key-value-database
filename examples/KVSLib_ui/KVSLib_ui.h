#ifndef KVSLIB_UI_H
#define KVSLIB_UI_H

// This new library builds on top of the ui designed for the local server

// Arguments delimiter
#define ARG_DELIM " "

// Establish connection corresponding command
#define CONNECT_CMD "connect"
// Put value corresponding command
#define PUT_CMD "put"
// Get value corresponding command
#define GET_CMD "get"
// Delete value corresponding command
#define DEL_CMD "del"
// Close connection corresponding command
#define CLOSE_CMD "close"

// Establish connection descriptor
#define CONNECT_DES 1
// Put value descriptor
#define PUT_DES 2
// Get value descriptor
#define GET_DES 3
// Delete value descriptor
#define DEL_DES 4
// Close connection descriptor
#define CLOSE_DES 6

// \brief Prints the various commands of the menu of the user interface
void printUI(void);

// Maximum input to be gotten from the keyboard
// Must be higher than the sum of MAX_GROUP_ID and the lengths of each the
// command and the delimiter
#define MAX_INPUT MAX_GROUP_LEN+20
// \brief Gets a command from the user and, in case of error, explains it
// \param groupName so that the function can communicate to the outside the 
// name of the group it received
// \return On error, returns 0 and, on success, a command descriptor
int getCommand(char *groupName);

#endif