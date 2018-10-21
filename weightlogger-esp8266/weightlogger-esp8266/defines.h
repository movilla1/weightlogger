#ifndef __DEFINES__
#define __DEFINES__ 0

#define READY 0
#define WPS_RUN 1
#define WPS_END 2
#define GET_IP_REQ 4
#define GET_TAG_REQ 8
#define GET_SERVER_IP 16
#define STATUS_REQ 32

#define MAX_SRV_CLIENTS 3
#define MAX_URL_LEN 200
#define SERVER_URL_STORAGE_ADDR 0
#define PASSWORD_STORAGE0 30 //max 20 chars per password.
#define PASSWORD_STORAGE1 60 //max 20 chars per password.
#define PASSWORD_MAX_LENGTH 20
#define IP_MAX_LENGTH 15
#define REMOTE_USR 1
#define ADMIN_USR 0

#define DASHBOARD 0
#define SETUP_IP_ADR 1
#define SETUP_TAGS 2
#define SETUP_PASS 3
#endif
