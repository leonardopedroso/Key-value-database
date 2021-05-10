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

// ---------- Data management ----------
// [POR ENQUANTO ESTAMOS A ETABELECER UM LIMITE DE CARACTERES MAS 
// PODE-SE NA BOA ALOCAR DINAMICAMENTE SEM LIMITE SE QUISERMOS DEPOIS]
// Data management parameters
#define MAX_STR_LENGTH 20

#endif