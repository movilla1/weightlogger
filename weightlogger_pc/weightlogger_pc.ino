#include <SPI.h>
#include <Wire.h>
#include <RF24.h>

#include "definitions.h"

byte addresses[][6] = {"ELCN1","ELCN2","ELCNM"};
RF24 radio(7, 8); //RF24 Radio on pins 7 & 8
byte sys_state;

void setup() {
  Wire.begin();
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 0);
  Serial.begin(28800, SERIAL_8N1);
  initialize_radio();
}

void loop() {
  if (Serial.available()) {
    serial_manager();
  }
  if (radio.available()) {
    radio_comm_manager();
  }
  delay(50); //sleep for 50 milliseconds each time.
}
