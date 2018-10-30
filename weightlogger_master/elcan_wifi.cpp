#include <Arduino.h>
#include "elcan_wifi.h"
#include <Wire.h>
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

bool ElcanWifi::begin(long speed) {
  char received[20];
  byte count;
  Serial.begin(speed, SERIAL_8N1);
  Serial.write("Q");
  Serial.flush();
  count = Serial.readBytesUntil("\r", received, INITIALIZATION_STR_LEN);
  empty_serial_buffer();
  received[INITIALIZATION_STR_LEN] = 0x00;
  if (strcmp(received, "INIOK") != 0) {
    _error = 100;
  }
  return (_error == 0);
}

void ElcanWifi::get_ip(char *result) {
  byte pos = 0;
  char tmp;
  empty_serial_buffer();
  Serial.write("I");
  Serial.flush();
  pos = Serial.readBytesUntil("\r", result, IP_ADDRESS_LEN);
  empty_serial_buffer();
  result[pos] = 0x00;
}

bool ElcanWifi::is_error() {
  return (_error > 0);
}

int ElcanWifi::write(char *data) {
  int pos = 0;
  int dataLen = strlen(data);
  
  Serial.write("T");
  while (pos < dataLen) {
    Serial.write(data[pos]);
    pos++;
  }
  Serial.write("\n");
  Serial.flush();
  return pos;
}

int ElcanWifi::available() {
  return Serial.available();
}

char ElcanWifi::poll() {
  char tmp;
  Serial.write("P");
  tmp = Serial.read();
  return tmp;
}

void ElcanWifi::readCardData(char *result, char len) {
  char pos;
  memset(result, 0, len);
  Serial.write("T");
  Serial.flush();
  pos = Serial.readBytesUntil("\r",result, len);
  result[pos] = 0x00;
}

void ElcanWifi::empty_serial_buffer() {
  while(Serial.available()) {
    char t = Serial.read();
  }
}
