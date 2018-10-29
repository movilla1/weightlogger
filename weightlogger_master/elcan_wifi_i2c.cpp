#include <Arduino.h>
#include "elcan_wifi_i2c.h"
#include <Wire.h>
#define DEBUG true
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

ElcanWifiI2C::ElcanWifiI2C() {
  _i2c_addr = 0;
  _error = 0;
}

bool ElcanWifiI2C::begin(int addr) {
  char received[32];
  byte count;
  Wire.begin();
  _i2c_addr = addr;
  Wire.beginTransmission(_i2c_addr);
  Wire.write("I");
  Wire.endTransmission();
  Wire.requestFrom(_i2c_addr, INITIALIZATION_STR_LEN);
  count = 0;
  while(Wire.available() && count < INITIALIZATION_STR_LEN) {
    received[count] = Wire.read();
    count ++;
  }
  received[count] = 0x00;
  if (strcmp(received, "INIOK")!=0) {
    _error = 100;
  }
  return (_error == 0);
}

void ElcanWifiI2C::get_ip(char *result) {
  byte pos = 0;
  char tmp;
  Wire.beginTransmission(_i2c_addr);
  Wire.write("G");
  Wire.endTransmission();
  Wire.requestFrom(_i2c_addr, IP_ADDRESS_LEN+1);
  while (Wire.available() && pos < IP_ADDRESS_LEN){
    tmp = Wire.read();
    if (tmp!=0 && tmp!=0xFF) {
     result[pos] = tmp;
     pos++;
    }
  }
  result[pos+1] = 0x00;
#ifdef DEBUG
  Serial.println(result);
#endif
}

bool ElcanWifiI2C::is_error() {
  return (_error > 0);
}

int ElcanWifiI2C::write(char *data) {
  int pos = 0;
  int dataLen = strlen(data);
  Wire.beginTransmission(_i2c_addr);
  Wire.write("T");
#ifdef DEBUG
    Serial.println(data);
#endif
  while (pos < dataLen) {
    Wire.write(data[pos]);
    pos++;
  }
  Wire.endTransmission();
  return pos;
}

int ElcanWifiI2C::available() {
  return Wire.available();
}

char ElcanWifiI2C::poll() {
  char tmp;
  Wire.beginTransmission(_i2c_addr);
  Wire.write("P");
  Wire.endTransmission();
  delay(2);
  Wire.requestFrom(_i2c_addr, 1);
  tmp = Wire.read();
  return tmp;
}

void ElcanWifiI2C::readCardData(char *result, char len) {
  char pos;
  memset(result, 0, len);
  Wire.beginTransmission(_i2c_addr);
  Wire.write("Q");
  Wire.endTransmission();
  Wire.requestFrom(_i2c_addr, 6);
  pos = 0;
  while(Wire.available()) {
    result[pos] = Wire.read();
    pos++;
  }
}
