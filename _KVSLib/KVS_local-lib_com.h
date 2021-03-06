#ifndef KVS_LOCAL_LIB_COM_H
#define KVS_LOCAL_LIB_COM_H

// Define server parameters
#define KVS_LOCAL_SERVER_ADDR "/tmp/KVSLocalServer"
#define MAX_LEN_CN_SERVER_ADDR 100

// ---------- Connectivity ----------
#define DISCONNECTED_SOCKET -1

// ---------- Query identification ----------
#define MSG_ID_ESTBL_CONN 0 // positive numbers
#define MSG_ID_PUT_VAL -1
#define MSG_ID_GET_VAL -2
#define MSG_ID_DEL_VAL -3
#define MSG_ID_REG_CB -4
#define MSG_ID_CLOSE_CONN -5

// ---------- Query status ----------
#define STATUS_OK_W_ARG 1
#define STATUS_OK 0
#define STATUS_ACCSS_DENIED -1
#define STATUS_GROUP_DSN_EXIST -2
#define STATUS_ALLOC_ERROR -3
#define STATUS_KEY_DSNT_EXIST -4
#define STATUS_AUTH_COM -5

#endif