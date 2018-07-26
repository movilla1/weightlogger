//#define WITH_SCALE //uncomment this to get the weight capability

/**
 * System Statuses
 *
 */
#define READY 1
#define ERROR_RFID 2
#define ERROR_INVALID 4
#define ERROR_WEIGHT 5
#define READ_RFID 8
#define READ_WEIGHT 16
#define OPEN_BARRIER 25
#define DENIED_ACCESS 29
#define DATA_LINK 32
#define TIMED_WAIT 64
#define SEND_DATA_OUT 128

/**
 * Ports used
 */
#define LED 13

/**
 * System Constants
 *
 */
#define ENTERING 1
#define LEAVING 2
#define WAITING_TIME 20 // Mandatory delay in seconds
#define MAX_WEIGHT_WAIT_TIME 3000 // 3 seconds

/**
 * RF Constants
 */
#define MAX_CARD_WAIT_TIME 10000 //10 seconds
