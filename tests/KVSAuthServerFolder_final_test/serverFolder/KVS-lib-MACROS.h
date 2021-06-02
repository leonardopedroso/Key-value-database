#ifndef KVS_LIB_MACROS_H
#define KVS_LIB_MACROS_H

// ---------- Query identification ----------
#define MSG_ID_ESTBL_CONN 0 // positive numbers
#define MSG_ID_PUT_VAL -1
#define MSG_ID_GET_VAL -2
#define MSG_ID_DEL_VAL -3
#define MSG_ID_REG_CB -4
#define MSG_ID_CLOSE_CONN -5

// ---------- Query status ----------
#define STATUS_OK 0
#define STATUS_ACCSS_DENIED -1

// ---------- Connectivity ----------
#define DISCONNECTED_SOCKET -1

// ---------- Data management ----------
#define MAX_STR_LENGTH 20
#define MAX_GROUP_LEN 256   // so that one avoids IP fragmentation when 
                            // transmitting group and secret
                            // This includes the terminator character
#define MAX_SECRET_LEN 256  // same reason as the above
                            // This also includes the terminator character
                            // check pp. 1190 of "The Linux Programming 
                            // Interface" by Kerrisk section to see the reason 
                            // to the limits used

#endif