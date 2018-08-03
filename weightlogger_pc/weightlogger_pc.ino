#include <SPI.h>
#include <Wire.h>
#include <RF24.h>

#include "definitions.h"
#include "rf_protocol.h"

byte addresses[][6] = {"ELCN1","ELCN2","ELCNM"};
RF24 radio(7, 8); //RF24 Radio on pins 7 & 8
byte sys_state;
ElcanProto protocolManager;
byte attemps;
void setup() {
  Wire.begin();
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 0);
  Serial.begin(38400, SERIAL_8N1);
  initialize_radio();
  sys_state = READY;
  attemps = 0;
}

void loop() {
  switch (sys_state)
  {
    case READY:
      if (Serial.available()) {
        sys_state = SERIAL_COMM;
      }
      if (radio.available()) {
        sys_state = RADIO_COMM;
      }
      break;
    case SERIAL_COMM:
      serial_manager();
      break;
    case RADIO_COMM:
      radio_comm_manager();
      break;
  }
}
