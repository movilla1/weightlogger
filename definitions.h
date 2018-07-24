/**
 * System Statuses
 *
 */
#define READY 1
#define ERROR_SD 2
#define ERROR_RFID 4
#define ERROR_RTC 5
#define ERROR_INVALID 7
#define READ_RFID 8
#define READ_RTC 16
#define READ_WEIGHT 32
#define OPEN_BARRIER 64
#define WRITE_RECORD 128
#define DATA_LINK 171
#define TIMED_WAIT 250

/**
 * Ports used
 */
#define LED 13
#define BARRERA 0
#define CHIP_SELECT_SD 4
#define RTC_CS 2

/**
 * System Constants
 *
 */
#define ENTERING 1
#define LEAVING 2
#define MAX_EEPROM_POSITION 200 * sizeof(struct card_block)
#define WAITING_TIME 20 // Mandatory delay in seconds

/**
 * RF Protocol Constants
 */
#define INVALID 1
#define VALID 4
#define MAX_CARD_WAIT_TIME 10000 //10 seconds