#include "elcan_lcd.h"
#include <Arduino.h>

ElcanLCDManager::ElcanLCDManager() {
  LiquidCrystal_I2C lcd(LCD_ADDRESS, 16,2);
  lcd.init();
}

void ElcanLCDManager::show_error(char error_code) {
	char blinks;
	lcd.clear();
	switch(error_code) {
		case ERROR_RFID:
		blinks = 2;
		lcd.print(FS("Error 502"));
		break;
		case ERROR_RTC:
		blinks = 3;
		lcd.print(FS("Error 501"));
		case ERROR_WIFI:
		blinks = 4;
		lcd.print(FS("Error 503"));
		break;
		default:
		lcd.print(FS("Error 500"));
		blinks = 1;
		break;
	}
	for (char i=0; i < blinks; i++) {
		tone(BUZZER, 1400, 300);
		delay(200);
	}
	delay(500); //wait 1/2 second between displays at least;
}

void ElcanLCDManager::show_ip(char *ipaddr) {
	char text[32];
	memset(text, 0x00, sizeof(text));
	strcat(text, "Station IP......");
	strcat(text, ipaddr);
	this->show_message(text);
	delay(2500); //2 1/2 seconds delay to read the ip
	lcd.clear();
}

void ElcanLCDManager::show_ready(char *dateString) {
	lcd.setCursor(0,0);
	lcd.print(dateString);
	lcd.setCursor(0,1);
	lcd.print(FS("Esperando..."));
}

void ElcanLCDManager::show_message(char *msg) {
	char tmp[17];
	char len;
	lcd.clear();
	lcd.backlight();
	this->backlightStart = millis();
	len = strlen(msg);
	if (len > 16) {
		lcd.setCursor(0,0);
		substring(msg, tmp, 0, 15);
		lcd.print(tmp);
		memset(tmp, 0, sizeof(tmp));
		lcd.setCursor(0,1);
		substring(msg, tmp, 16, len);
		lcd.print(tmp);
	} else {
		lcd.print(msg);
	}
}

void ElcanLCDManager::check_light() {
	if (this->backlightStart > 0) {
		if (millis() - backlightStart > LIGHT_DURATION) {
			lcd.noBacklight();
			this->backlightStart = 0;
		}
	}
}

void ElcanLCDManager::light_on() {
	lcd.backlight();
	this->backlightStart = millis();
}
