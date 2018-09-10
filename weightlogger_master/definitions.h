/**
 * System Statuses
 *
 */
#define READY 1
#define ERROR_WIFI 2
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
#define WPS_BUTTON 2
#define BUZZER 3
#define BARRERA 4
#define WIFI_RX 5
#define WIFI_TX 6
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
#define LCD_ADDRESS 0x27
#define LIGHT_DURATION 4000 // 4 seconds with the light on.
/**
 * RF Protocol Constants
 */
#define MAX_CARD_WAIT_TIME 10000 //10 seconds

#define WIFI_I2C_ADDR 0x18
