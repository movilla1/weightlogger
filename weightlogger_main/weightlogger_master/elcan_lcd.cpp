#include "elcan_lcd.h"
#include <Arduino.h>
ElcanLCDManager::ElcanLCDManager(char addr, char cols, char rows) {
  this->lcd = LiquidCrystal_I2C();  // set the LCD address to 0x27 for a 16 chars and 2 line display
  this->lcd.init(addr, cols, rows);
}

void ElcanLCDManager::begin(char cols, char rows) {
  this->lcd.begin(cols, rows);
}

void ElcanLCDManager::show_error(char error_code) {
	char blinks;
	this->lcd.clear();
	switch(error_code) {
		case ERROR_RFID:
		blinks = 2;
		this->lcd.print(FS("Error 502"));
		break;
		case ERROR_RTC:
		blinks = 3;
		this->lcd.print(FS("Error 501"));
		case ERROR_WIFI:
		blinks = 4;
		this->lcd.print(FS("Error 503"));
		break;
		default:
		this->lcd.print(FS("Error 500"));
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
	this->lcd.clear();
}

void ElcanLCDManager::show_ready(char *dateString) {
	this->lcd.setCursor(0,0);
	this->lcd.print(dateString);
	this->lcd.setCursor(0,1);
	this->lcd.print(FS("Esperando..."));
}

void ElcanLCDManager::show_message(char *msg) {
	char tmp[17];
	char len;
	lcd.clear();
	lcd.backlight();
	this->backlightStart = millis();
	len = strlen(msg);
	if (len > 16) {
		this->lcd.setCursor(0,0);
		substring(msg, tmp, 0, 15);
		this->lcd.print(tmp);
		memset(tmp, 0, sizeof(tmp));
		this->lcd.setCursor(0,1);
		substring(msg, tmp, 16, len);
		this->lcd.print(tmp);
	} else {
		this->lcd.print(msg);
	}
}

void ElcanLCDManager::check_light() {
	if (this->backlightStart > 0) {
		if (millis() - backlightStart > LIGHT_DURATION) {
			this->lcd.noBacklight();
			this->backlightStart = 0;
		}
	}
}

void ElcanLCDManager::light_on() {
	this->lcd.backlight();
	this->backlightStart = millis();
}
