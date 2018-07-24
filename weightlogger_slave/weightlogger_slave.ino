#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>  // Library for Mifare RC522 Devices
#include <RF24.h>

#include "definitions.h"

MFRC522 mfrc522;
RF24 radio(7, 8); //RF24 Radio on pins 7 & 8
byte readCard[4];   // Stores scanned ID read from RFID Module
byte addresses[][6] = {"ELCN1","ELCN2","ELCNM"};
byte sys_state;

void setup() {
  Wire.begin();
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 0);
  Serial.begin(4800, SERIAL_8N1); // according to wheight measurement device
  initialize_radio();
  mfrc522.PCD_Init();                             // Initialize MFRC522 Hardware
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max); // Max reading distance
  sys_state = READY;
}

void loop() {
  switch(sys_state) {
    case ERROR_SD:
    case ERROR_RFID:
    case ERROR_RTC:
      show_error(sys_state);
      break;
    case READY:
      if (mfrc522.PICC_IsNewCardPresent()) {
        sys_state = READ_RFID;
      }
      if (radio.available()){
        sys_state = DATA_LINK;
      }
    case READ_RFID:
      if (read_rfid_value()) {
        check_card_and_act(); //checks the card and if its valid, it starts the sequence
      }
      break;
    case OPEN_BARRIER:
      open_barrier();
      sys_state = READY;
      break;
    case DATA_LINK:
      rf_protocol_manager();
      break;
  }
}