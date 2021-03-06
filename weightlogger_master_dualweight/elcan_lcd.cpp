#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include "includes/definitions.h"
#include "includes/elcan_lcd.h"
#include "includes/substring.h"

ElcanLCDManager::ElcanLCDManager() : LiquidCrystal_I2C(LCD_ADDRESS, 16, 2) {
  // constructor initialized
}

void ElcanLCDManager::init() {
  uint8_t copyright[8] = {	
    0x0E, 0x11,
    0x06, 0x08,
    0x08, 0x06,
    0x11, 0x0E
  };
  uint8_t logo[8] = {	
    0x1F, 0x1F,
    0x18, 0x17,
    0x0F, 0x18,
    0x1F, 0x1F
  };
  LiquidCrystal_I2C::init();
  createChar(0, copyright);
  createChar(1, logo);
}

void ElcanLCDManager::welcome_message() {
  clear();
  backlight();
  setCursor(0,0);
  print(F("WeightLogger"));
  setCursor(0,1);
  write(0);
  print(F("2019 - "));
  write(1);
  print(F(" v0.7.0"));
}

size_t Print::write(uint8_t b) {
  print(b);
}

void ElcanLCDManager::show_error(char error_code) {
  char blinks;
  clear();
  switch(error_code) {
    case ERROR_RFID:
    blinks = 2;
    print(F("Error 502"));
    break;
    case ERROR_RTC:
    blinks = 3;
    print(F("Error 501"));
    case ERROR_WIFI:
    blinks = 4;
    print(F("Error 503"));
    break;
    default:
    print(F("Error 500"));
    blinks = 1;
    break;
  }
  for (char i=0; i < blinks; i++) {
    tone(BUZZER, 1400, 200);
    delay(100);
  }
  delay(500); //wait 1/2 second between displays at least;
}

void ElcanLCDManager::show_ip(char *ipaddr) {
  backlight();
  backlightStart = millis();
  clear();
  setCursor(0,0);
  print(F("Station IP..."));
  setCursor(0,1);
  print(ipaddr);
  delay(2500); //2 1/2 seconds delay to read the ip
  clear();
}

void ElcanLCDManager::show_ready(char *dateString) {
  setCursor(0,0);
  print(dateString);
  setCursor(0,1);
  print(F("Esperando..."));
}

void ElcanLCDManager::show_message(char *msg) {
  char tmp[17];
  char len;
  clear();
  backlight();
  backlightStart = millis();
  len = strlen(msg);
  if (len > 16) {
    setCursor(0,0);
    substring(msg, tmp, 0, 15);
    print(tmp);
    memset(tmp, 0, sizeof(tmp));
    setCursor(0,1);
    substring(msg, tmp, 16, len);
    print(tmp);
  } else {
    print(msg);
  }
}

void ElcanLCDManager::check_light() {
  if (backlightStart > 0) {
    if (millis() - backlightStart > LIGHT_DURATION) {
      clear();
      noBacklight();
      backlightStart = 0;
    }
  }
}

void ElcanLCDManager::light_on() {
  backlight();
  backlightStart = millis();
}
