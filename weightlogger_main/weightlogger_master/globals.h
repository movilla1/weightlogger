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
#include "scale_i2c.h"
#include "definitions.h"
#include "card_format.h"
#include "card_manager.h"

char sys_state;
char measuredWeight[7]; // Stores weight in ram
DateTime enteringTime;  //last time readed on the RTC
DateTime timerStarted;
long backlightStart;
long lastPoll;
ElcanLCDManager ElcanLcd(LCD_ADDRESS, 16, 2);
ElcanWifi wifi;
ElcanRtc rtc;
RfidManager rfid;
#ifdef WITH_WEIGHT
ElcanScale scale;
#endif

#define _GLOBALS_ 1
#endif
