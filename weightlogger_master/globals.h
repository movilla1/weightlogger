/**
 * Global Variables
 */
#ifndef _GLOBALS_
#include "definitions.h"

RTC_DS3231 rtc;
MFRC522 mfrc522(RFID_SS, RFID_RST); //Creamos el objeto para el RC522
LiquidCrystal_I2C lcd(LCD_ADDRESS,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

byte sys_state;
byte readCard[4];   // Stores scanned ID read from RFID Module
char measuredWeight[7]; // Stores weight in ram
DateTime enteringTime;  //last time readed on the RTC
DateTime timerStarted;
long backlightStart;
long lastPoll;
ElcanWifi wifi;

struct card_block {
  byte card_uid[4];
  byte card_number;
};
#define _GLOBALS_
#endif
