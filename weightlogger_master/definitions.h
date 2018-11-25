/**
 * System Statuses
 *
 */
#define READY         1
#define ERROR_WIFI    2
#define ERROR_RFID    4
#define ERROR_RTC     5
#define ERROR_INVALID 7
#define READ_RFID     8
#define READ_RTC      16
#define READ_WEIGHT   32
#define READ_WEIGHT2  40
#define OPEN_BARRIER  64
#define WRITE_RECORD  128
#define UNKNOWN_CARD  137
#define GET_TAG_DATA  171
#define ADJUST_TIME   193
#define TIMED_WAIT    250

/**
 * Ports used
 */
#define PUSH_BUTTON 2
#define BUZZER    3
#define BARRERA   4
#define WIFI_RX   5
#define WIFI_TX   6
#define RFID_RST  9    //Pin 9 para el reset del RC522
#define RFID_SS   10  //Pin 10 para el SS (SDA) del RC522
/**
 * System Constants
 *
 */
#define CARD_UID_SIZE 4
#define STARTUP_DELAY 9000
#define BARRIER_DURATION 1200
#define MAX_EEPROM_POSITION 200 * sizeof(struct card_block)
#define WAITING_TIME 5 // Mandatory delay in seconds
#define MAX_WEIGHT_WAIT_TIME 3000 // 3 seconds
#define LCD_ADDRESS 0x27
#define LIGHT_DURATION 5000 // 5 seconds with the light on.
#define POLLING_INTERVAL 1000
#define TAG_UID_START 0
#define TAG_UID_END 8
#define TAG_POS_START 8
#define TAG_POS_END 10
#define TAG_REM_START 10
#define TAG_PACKET_SIZE 11 // tag example: 96484C4F, 00-C8 (0 to 200), 0/1
#define SCALE_I2C_ADDR 0x42

#define CLEAR_SERIAL while(Serial.available()) { byte t = Serial.read(); }