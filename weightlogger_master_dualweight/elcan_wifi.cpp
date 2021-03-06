#include <Arduino.h>
#include "includes/elcan_wifi.h"
#include <Wire.h>
//#define DEBUG 1
/**
 * This library requires the Wire library included and initialized.
 * @author: Mario O. Villarroel
 * @copyright: 2018 - ElcanSoftware
 * This works for the following protocol:
 *  'G': //get IP address
 *  'T': //Transmit data to Server
 *  'I': //initialization, returns INIOK
 *  'P': //poll status, returns T, I, S
 *  'Q': //get tag, gets the tag data (6 bytes, 4 uid, 1 position, 1 rmv status where 0 keeps and 1 removes)
 * */

ElcanWifi::ElcanWifi() {
  _error = 0;
  _error = false;
}

bool ElcanWifi::begin() {
  char received[20];
  char count;
  empty_serial_buffer();
  Serial.write("Q");
  Serial.flush();
  count = Serial.readBytesUntil('\r', received, INITIALIZATION_STR_LEN);
  empty_serial_buffer();
  received[INITIALIZATION_STR_LEN] = 0x00;
  if (strcmp(received, "INIOK") != 0) {
    _error = 100;
  }
  delay(1);
  empty_serial_buffer();
  return (_error == 0);
}

void ElcanWifi::get_ip(char *result) {
  char pos = 0;
  char tmp;
  empty_serial_buffer();
  Serial.write("I");
  Serial.flush();
  pos = Serial.readBytesUntil('\r', result, IP_ADDRESS_LEN);
  result[pos] = 0x00;
}

bool ElcanWifi::is_error() {
  return (_error > 0);
}

char ElcanWifi::sendEntry(char *data) {
  char tmpText[50];
  char len;
  memset(tmpText, 0, sizeof(tmpText));
  tmpText[0]='S';
  len = strlen(data);
  memcpy(tmpText+1, data, len);
  #ifdef DEBUG
  Serial.print("#");
  Serial.println(tmpText);
  #endif
  for (char b=0; b <= len; b++) {
    Serial.write(tmpText[b]);
  }
  Serial.write(EOL);
  Serial.flush();
  return len;
}

void ElcanWifi::sendIntrussionAttemp(char *data) {
  char text[32];
  char len = strlen(data);
  memset(text,0,sizeof(text));
  text[0]='N';
  memcpy(text+1, data, len); // 8 bytes: 4 cardID, 4 timestamp
  for (char b=0; b <= len; b++) {
    Serial.write(text[b]);
  }
  Serial.write(EOL);
  Serial.flush();
}

int ElcanWifi::available() {
  return Serial.available();
}

char ElcanWifi::poll() {
  char tmp;
  char count = 0;
  empty_serial_buffer();
  Serial.write("P");
  Serial.flush();
  while(!Serial.available() && count++ < 255);
  tmp = Serial.read();
  return tmp;
}

void ElcanWifi::readCardData(char *result, char maxLen) {
  char pos;
  memset(result, 0, maxLen);
  Serial.write("T");
  Serial.flush();
  delay(1);
  pos = Serial.readBytesUntil('\r', result, maxLen);
  empty_serial_buffer();
  result[pos] = 0x00;
}

void ElcanWifi::empty_serial_buffer() {
  char t;
  while(Serial.available()) {
    t = Serial.read();
  }
}
