/**
 * Global Variables
 */
#ifndef _GLOBALS_
#include "definitions.h"

RTC_DS3231 rtc;
MFRC522 mfrc522(RFID_SS, RFID_RST); //Creamos el objeto para el RC522
LiquidCrystal_I2C lcd(LCD_ADDRESS,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
//SoftwareSerial wifi(WIFI_RX, WIFI_TX);
byte sys_state;
byte readCard[4];   // Stores scanned ID read from RFID Module
byte whos_entering; //stores in ram the card position that's readed
uint16_t measuredWeight; // Stores weight in ram
DateTime enteringTime;  //last time readed on the RTC
DateTime timerStarted;
long backlightStart;
ElcanWifiI2C wifi;

struct card_block {
  byte card_uid[4];
  byte card_number;
};
#define _GLOBALS_
#endif
