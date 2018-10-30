#include <Wire.h>
#include <Arduino.h>
#include "scale_i2c.h"

ElcanScale::ElcanScale() {
  _error = 0;
  _i2c_address = 0;
}

bool ElcanScale::begin(int addr) {
  char tmp[8];
  char pos = 0;
  memset(tmp, 0, sizeof(tmp));
  _i2c_address = addr;
  Wire.beginTransmission(_i2c_address);
  Wire.write("I"); //send INIT
  Wire.endTransmission();
  Wire.requestFrom(_i2c_address, 6);
  while (Wire.available()) {
    char b = Wire.read();
    if (b != 0xFF) {
      tmp[pos] = b;
      pos ++;
    }
  }
  tmp[pos+1] = 0x00;
  if (strcmp(tmp, "SCALEOK") != 0) {
    _error = 100;
  }
  return (_error == 0);
}

uint16_t ElcanScale::get_weight() {
  // TODO: Write it return atoi(str);
}

bool ElcanScale::is_error() {
  return (_error != 0);
}
