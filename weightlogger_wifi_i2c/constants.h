#define IP_START_ADDR 0x00
#define IP_START_POS_IN_RESPONSE 14
#define WIFI_TIMEOUT 2000 //milliseconds timeout for wifi
#define WPS_TIMEOUT 30000 // 30 seconds after turning on wps we turn it off
#define BUFFER_SIZE 24
#define IPD_BUFFER_SIZE 250
#define IP_ADDR_SIZE 15
#define PARAM_SIZE 15
#define SKIP_SIZE 4 // skip ?IP= OR ?TA=
#define COMMAND_SIZE 6

#define SETUP_BTN 3
#define LED 8
#define I2C_ADDR 0x18

#define READY 0
#define WPS_SETUP 1
#define I2C_GET 2
#define ERROR_WIFI 4
#define WPS_SETUP_STARTED 6
#define WPS_ON 8
#define SEND_POLL_DATA 16
#define GET_TAG_DATA 22
#define SEND_INIT_DATA 32
#define SEND_SERVER_IP 64
#define SEND_IP_ADDRESS 128
