#ifndef KVSLIB_UI_H
#define KVSLIB_UI_H

// This new library builds on top of the ui designed for the local server

// Arguments delimiter
#define ARG_DELIM " "
// Maximum number of arguments for every command
#define MAX_ARGS 2

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

// \brief Prints the various commands of the menu of the user interface
void printUI(void);

// Maximum input to be gotten from the keyboard
// Must be higher than the sum of MAX_GROUP_LEN, MAX_SECRET_LEN and the lengths
// of each the command and the delimiter
#define MAX_INPUT MAX_GROUP_LEN+MAX_SECRET_LEN+20

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
// Success return value
#define SUCCESS 0
// Error on fgets return value
#define ERR_FGETS -1
// Error on input too large return value
#define ERR_TOO_LARGE -2
// Allocation error return value
#define ERR_ALLOC -3
// No argument given return value
#define EMPTY -4
// Invalid command descriptor
#define INV_DES -5

// \brief Gets a command from the user and, in case of error, explains it
// \param args arguments of the commands
// \return Returns one of the above values
int getCommand(char **args);

#endif