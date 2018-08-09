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
#define UNKNOWN_CARD 137
#define DATA_LINK 171
#define TIMED_WAIT 250

/**
 * Ports used
 */
#define BUZZER 2
#define LED 3
#define BARRERA 5
#define SDCARD_SS 4
#define RADIO_CE 7
#define RADIO_SS 8
#define RFID_RST  9    //Pin 9 para el reset del RC522
#define RFID_SS   10  //Pin 10 para el SS (SDA) del RC522
/**
 * System Constants
 *
 */
#define ENTERING 1
#define LEAVING 2
#define MAX_EEPROM_POSITION 200 * sizeof(struct card_block)
#define WAITING_TIME 10 // Mandatory delay in seconds
#define MAX_WEIGHT_WAIT_TIME 3000 // 3 seconds
#define MAX_BYTES_WRONG 1024 // if we can't get a steady weight after 1K bytes received then timeout.
/**
 * RF Protocol Constants
 */
#define MAX_CARD_WAIT_TIME 10000 //10 seconds

/**
 * SD card attached to SPI bus as follows:
 * MOSI - pin 11
 * MISO - pin 12
 * CLK - pin 13
 * CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
 */

#define SPI_SDCARD 1
#define SPI_RADIO 2
#define SPI_RFID 4
