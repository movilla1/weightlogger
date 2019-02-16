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
#define READ_WEIGHT2  48
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
#define BUZZER      3
#define BARRERA     4
#define WIFI_RX     5
#define WIFI_TX     6

/**
 * System Constants
 *
 */
#define CARD_UID_SIZE 4
#define MAX_EEPROM_POSITION 200 * sizeof(struct card_block)
#define WAITING_TIME 10 // Mandatory delay in seconds
#define MAX_WEIGHT_WAIT_TIME 3000 // 3 seconds
#define LCD_ADDRESS 0x3F
#define LIGHT_DURATION 4000 // 4 seconds with the light on.
#define POLLING_INTERVAL 1000
#define TAG_UID_START 0
#define TAG_UID_END 8
#define TAG_POS_START 8
#define TAG_POS_END 10
#define TAG_REM_START 10
#define TAG_PACKET_SIZE 11 // tag example: 96484C4F, 00-C8 (0 to 200), 0/1
#define SCALE_I2C_ADDR 0x42

/************************************************************************/
/* Macros                                                               */
/************************************************************************/
#define CLEAR_SERIAL while(Serial.available()) { byte t = Serial.read(); }
#define DO_KNOWN_BEEPS for(char b=0; b<2; b++){tone(BUZZER, 800, 100); delay(300);}
#define DO_INTRUSSION_BEEPS for(byte i=0; i<3; i++) {tone(BUZZER, 1500, 200);delay(350);}
#define DO_OPEN_BARRIER digitalWrite(BARRERA, 1); delay(1200); digitalWrite(BARRERA, 0);