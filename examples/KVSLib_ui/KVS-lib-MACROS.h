#ifndef KVS_LIB_MACROS_H
#define KVS_LIB_MACROS_H

// ---------- Data management ----------
#define MAX_GROUP_LEN 256   // so that one avoids IP fragmentation when 
                            // transmitting group and secret
                            // This includes the terminator character
#define MAX_SECRET_LEN 256  // same reason as the above
                            // This also includes the terminator character
                            // check pp. 1190 of "The Linux Programming 
                            // Interface" by Kerrisk section to see the reason 
                            // to the limits used

#endif