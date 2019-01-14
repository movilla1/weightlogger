#include <Wire.h>
#include <Arduino.h>
#include "elcan_scale.h"

#define DEBUG 1

ElcanScale::ElcanScale(byte addr) {
  _error = 0;
  _i2c_address = addr;
}

bool ElcanScale::begin() {
  char tmp[8];
  char pos = 0;
  memset(tmp, 0, sizeof(tmp));
  Wire.beginTransmission(_i2c_address);
  Wire.write("I"); //send INIT
  Wire.endTransmission();
  delay(1);
  Wire.requestFrom(_i2c_address, 6u);
  while (Wire.available()) {
    char b = Wire.read();
    if (b != 0xFF) {
      tmp[pos] = b;
      pos ++;
    }
  }
  tmp[pos+1] = 0x00;
  #ifdef DEBUG
  Serial.print("#");
  Serial.println(tmp);
  #endif
  if (strcmp(tmp, "SCALEK") != 0) {
    _error = 100;
  }
  return (_error == 0);
}

void ElcanScale::get_weight(byte *dest) {
  byte temp[8];
  bool exit, error = false;
  byte attemps = 0;
  memset(temp, 0, sizeof(temp));
  exit = false;
  while(!exit) {
    if (attemps > 3) {
      exit = true;
      error = true;
    } else {
      attemps++;
      exit = do_get_weight(temp);
      #ifdef DEBUG
      Serial.print("#attemps:");
      Serial.println(attemps, DEC);
      #endif
    }
  }
  #ifdef DEBUG
  Serial.print("#scale:");
  Serial.println((char *)temp);
  #endif
  if (error == false) {
    memcpy(dest, temp, sizeof(temp));
  } else {
    strcat((char *)dest, "000000");
  }
}

bool ElcanScale::do_get_weight(byte *dst) {
  byte tmp[8];
  char pos = 0;
  char b;
  Wire.beginTransmission(_i2c_address);
  Wire.write("W"); //send INIT
  Wire.endTransmission();
  delay(1);
  Wire.requestFrom(_i2c_address, 7u);
  while (Wire.available()) {
    b = Wire.read();
    if (b != 0xFF) {
      tmp[pos++] = b;
    }
  }
  tmp[pos+1] = 0x00;
  #ifdef DEBUG
  Serial.print("#SCALE:");
  Serial.println((char *)tmp);
  #endif
  if (pos > 1) {
    memcpy(dst, tmp, pos);
  }
  return (pos > 1);
}
bool ElcanScale::is_error() {
  return (_error != 0);
}
