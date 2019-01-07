/**
 * Global Variables
 */
#ifndef _GLOBALS_
#include <Wire.h>
#include <SPI.h>
#include "rtc.h"
#include "rfid.h"
#include "elcan_lcd.h"
#include "converters.h"
#include "elcan_wifi.h"
#include "elcan_scale.h"
#include "definitions.h"
#include "card_format.h"
#include "card_manager.h"

byte sys_state;
DateTime enteringTime;  //last time readed on the RTC
DateTime timerStarted;
long backlightStart;
long lastPoll;
ElcanLCDManager elcanLcd;
ElcanRtc rtc;
MFRC522 rfidObj(RFID_SS, RFID_RST);
RfidManager rfid(&rfidObj);
volatile bool buttonPressed;

#ifdef WITH_WIFI
ElcanWifi wifi;
#endif

#ifdef WITH_WEIGHT
byte measuredWeight[7]; // Stores weight in ram
byte secondWeight[7];
ElcanScale scale;
#endif

#define _GLOBALS_ 1
#endif
