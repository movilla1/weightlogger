#include "constants.h"

/**
 * Module file to handle everything wifi related, with the AT command firmware on ESP8266
 **/

void initialize_wifi() {
  bool timeout=false;
  long tstart=0;
  Serial.print(F("Q\r\n"));
  Serial.flush();
  delay(5);
  tstart = millis();
  while (!Serial.available() && !timeout) {
    timeout = ((millis() - tstart) < WIFI_TIMEOUT);    
  }
  if (timeout) {
    sysState = ERROR_WIFI;
  } else {
    empty_serial_buffer();
  }
}

void send_wifi_wps_setup() {
  char tmp[5];
  Serial.print(F("W\r\n"));
  Serial.flush();
  delay(5);
  empty_serial_buffer();
}

void get_ip_address() {
  byte count = 0;
  char t;
  pollData = 'I';
  empty_serial_buffer();
  Serial.print(F("I\r\n"));
  Serial.flush();
  delay(1);
  while(Serial.available() && count < IP_START_POS_IN_RESPONSE + IP_ADDR_SIZE) {
    count++;
    if (count > IP_START_POS_IN_RESPONSE) { //
      t = Serial.read();
      wireBuffer[count - IP_START_POS_IN_RESPONSE] = t;
    }
  }
  empty_serial_buffer(); //drop the rest.
  count = count - IP_START_POS_IN_RESPONSE;
  wireBuffer[count] = 0x00; //end string at the last position.
}

void transmit_to_server() {
  char t;
  Serial.print(F("T"));
  while(Wire.available()) {
    t = Wire.read();
    Serial.write(t);
  }
  Serial.print(F(" \r\n"));
  Serial.flush();
}
