/**
 * System Statuses
 *
 */
#define READY 1
#define ERROR_RFID 2
#define ERROR_INVALID 4
#define READ_RFID 8
#define DATA_LINK 16
#define TIMED_WAIT 32

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

/**
 * RF Protocol Constants
 */
#define INVALID 1
#define VALID 4
#define MAX_CARD_WAIT_TIME 10000 //10 seconds
