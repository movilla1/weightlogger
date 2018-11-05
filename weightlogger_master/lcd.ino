#include "globals.h"

void lcd_show_ready() {
  char dateString[15];
  lcd.setCursor(0,0);
  DateTime tstamp = rtc.now();
  sprintf(dateString, "%02d/%02d/%04d %02d:%02d", tstamp.day(), tstamp.month(),
    tstamp.year(), tstamp.hour(), tstamp.minute());
  lcd.print(dateString);
  lcd.setCursor(0,1);
  lcd.print(F("Esperando..."));
}

void lcd_show_ip() {
#ifdef WITH_WIFI  
  char ipaddress[18];
  char text[32];
  memset(ipaddress,0x00 ,sizeof(ipaddress));
  memset(text, 0x00, sizeof(text));
  strcat(text, "Station IP......");
  wifi.get_ip(ipaddress);
  strcat(text, ipaddress);
  lcd_show_message(text);
  delay(2500); //2 1/2 seconds delay to read the ip
  lcd.clear();
#else
  lcd_show_message("Initialized");
#endif
}

void lcd_show_allowed() {
  lcd_show_message(F("Acceso permitido"));
}

void lcd_show_wait() {
  lcd_show_message(F("Espere por favor..."));
}

void lcd_show_go() {
  lcd_show_message(F("Avance..."));
}

void lcd_show_message(String message) {
  byte len = 0;
  lcd.clear();
  lcd.backlight();
  backlightStart = millis();
  String msg = message;
  if (msg.length() > 16) {
    lcd.setCursor(0,0);
    lcd.print(msg.substring(0,16));
    lcd.setCursor(0,1);
    lcd.print(msg.substring(16, msg.length()));
  } else {
    lcd.print(msg);
  }
}

void check_lcd_light() {
  if (backlightStart > 0) {
    if (millis() - backlightStart > LIGHT_DURATION) {
      lcd.noBacklight();
      backlightStart = 0;
    }
  }
}

void lcd_light_on() {
  lcd.backlight();
  backlightStart = millis();
}

void debug_lcd(char *data, char datalen) {
  lcd.clear();
  lcd.backlight();
  for (byte p=0; p < datalen; p++) {
    lcd.print(data[p]);
  }
  delay(3000);
}