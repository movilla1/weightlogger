#include "constants.h"

/**
 * Module file to handle everything wifi related, with the AT command firmware on ESP8266
 **/

void initialize_wifi() {
  bool timeout=false;
  long tstart=0;
  Serial.print(F("Q\r\n"));
  Serial.flush();
  delay(3);
  tstart = millis();
  while (!Serial.available() && !timeout) {
    timeout = ((millis() - tstart) < WIFI_TIMEOUT);    
  }
  if (timeout) {
    sysState = ERROR_WIFI;
  }
}

void send_wifi_wps_setup() {
  char tmp[5];
  Serial.print(F("W\r\n"));
  Serial.flush();
}

void get_ip_address() {
  char t;
  pollData = 'I';
  Serial.print(F("I\r\n"));
  Serial.flush();
}

void transmit_to_server() {
  char t;
  Serial.print(F("T"));
  while(Wire.available()) {
    t = Wire.read();
    Serial.write(t);
  }
  Serial.flush();
}

void getTagData() {
  char tmp[20];
  char pos = 0;
  memset(tmp, 0x00, sizeof(tmp));
  Serial.print(F("T\r\n"));
  Serial.flush();
}
