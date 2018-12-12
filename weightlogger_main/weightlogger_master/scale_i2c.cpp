#include <Wire.h>
#include <Arduino.h>
#include "scale_i2c.h"

ElcanScale::ElcanScale() {
  _error = 0;
  _i2c_address = 0;
}

bool ElcanScale::begin(byte addr) {
  char tmp[8];
  char pos = 0;
  memset(tmp, 0, sizeof(tmp));
  _i2c_address = addr;
  Wire.beginTransmission(_i2c_address);
  Wire.write("I"); //send INIT
  Wire.endTransmission();
  Wire.requestFrom(_i2c_address, 6u);
  while (Wire.available()) {
    char b = Wire.read();
    if (b != 0xFF) {
      tmp[pos] = b;
      pos ++;
    }
  }
  tmp[pos+1] = 0x00;
  if (strcmp(tmp, "SCALEK") != 0) {
    _error = 100;
  }
  return (_error == 0);
}

void ElcanScale::get_weight(char *dest) {
  char tmp[8];
  char pos = 0;
  Wire.beginTransmission(_i2c_address);
  Wire.write("W"); //send INIT
  Wire.endTransmission();
  Wire.requestFrom(_i2c_address, 7u);
  while (Wire.available()) {
    char b = Wire.read();
    if (b != 0xFF) {
      tmp[pos] = b;
      pos ++;
    }
  }
  tmp[pos+1] = 0x00;
  memcpy(dest, tmp, pos);
}

bool ElcanScale::is_error() {
  return (_error != 0);
}
