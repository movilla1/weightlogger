#include <Arduino.h>
#include "elcan_wifi_i2c.h"
#include <Wire.h>
/**
 * This library requires the Wire library included and initialized.
 * @author: Mario O. Villarroel
 * @copyright: 2018 - ElcanSoftware
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
  delay(20); //20 ms wait for response
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

void ElcanWifiI2C::set_server_ip(char *ip) {
  bool finish = false;
  byte pos = 0;
  Wire.beginTransmission(_i2c_addr);
  Wire.write("T");
  while(!finish) {
    Wire.write(ip[pos]);
    if (ip[pos]==0x00) {
      finish = true;
    }
    pos ++;
  }
  Wire.endTransmission();
}

char *ElcanWifiI2C::get_ip() {
  char result[32];
  byte pos = 0;
  Wire.beginTransmission(_i2c_addr);
  Wire.write("G");
  Wire.endTransmission();
  delay(200);
  Wire.requestFrom(_i2c_addr, IP_ADDRESS_LEN); 
  while (Wire.available() && pos < IP_ADDRESS_LEN){
    result[pos] = Wire.read();
    pos++;
  }
  result[pos] = 0x00;
  return result;
}

bool ElcanWifiI2C::is_error() {
  return (_error > 0);
}

int ElcanWifiI2C::write(char *data) {
  int pos = 0;
  int dataLen = strlen(data);
  Wire.beginTransmission(_i2c_addr);
  Wire.write("T");
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
