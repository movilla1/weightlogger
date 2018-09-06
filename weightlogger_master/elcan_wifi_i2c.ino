#include <Arduino.h>
#include <string.h>
#include "elcan_wifi_i2c.h"
/**
 * This library requires the Wire library included and initialized.
 * @author: Mario O. Villarroel
 * @copyright: 2018 - ElcanSoftware
 * */

ElcanWifiI2C::ElcanWifiI2C() {
  _i2c_addr = 0;
  _error = 0;
}

void ElcanWifiI2C::begin(byte addr) {
  String init_string;
  byte count;
  _i2c_addr = addr;
  Wire.beginTransmission(_i2c_addr);
  Wire.write("I");
  Wire.endTransmission();
  delay(20); //20 ms wait for response
  Wire.requestFrom(_i2c_addr, INITIALIZATION_STR_LEN);
  count = 0;
  while(Wire.available() && count < INITIALIZATION_STR_LEN) {
    init_string.concat( Wire.read() );
    count ++;
  }
  if (!init_string.equals("INIOK")) {
    _error = 100;
  }
}

void ElcanWifiI2C::set_server_ip(char *ip) {
  bool finish = false;
  byte pos = 0;
  Wire.beginTransmission(_i2c_addr);
  Wire.write(F("T"));
  while(!finish) {
    Wire.write(ip[pos]);
    if (ip[pos]==0x00) {
      finish = true;
    }
    pos ++;
  }
  Wire.endTransmission();
}

String ElcanWifiI2C::get_ip_address() {
  String result;
  byte pos = 0;
  Wire.beginTransmission(_i2c_addr);
  Wire.write("G");
  Wire.endTransmission();
  delay(20);
  Wire.requestFrom(_i2c_addr, IP_ADDRESS_LEN); 
  while (Wire.available() && pos < IP_ADDRESS_LEN){
    result.concat(Wire.read());
    pos++;
  }
  return result;
}

bool ElcanWifiI2C::is_error() {
  return (_error > 0);
}

int ElcanWifiI2C::write(String data) {
  int pos = 0;
  Wire.beginTransmission(_i2c_addr);
  Wire.write("T");
  while (pos < data.length()) {
    Wire.write(data.charAt(pos));
    pos++;
  }
  Wire.endTransmission();
  return pos;
}