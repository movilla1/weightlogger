#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>  // Library for Mifare RC522 Devices
#include <RF24.h>

#include "definitions.h"

MFRC522 mfrc522;
RF24 radio(7, 8); //RF24 Radio on pins 7 & 8
byte readCard[4];   // Stores scanned ID read from RFID Module
byte addresses[][6] = {"ELCN1","ELCN2","ELCNM"};
byte whos_entering; //stores in ram the card position that's readed
byte sys_state;
uint16_t measured_weight;

void setup() {
  Wire.begin();
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 0);
  Serial.begin(4800, SERIAL_8N1); // according to wheight measurement device
  initialize_radio();
  mfrc522.PCD_Init();                             // Initialize MFRC522 Hardware
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max); // Max reading distance
  measured_weight = -1;
  sys_state = READY;
}

void loop() {
  switch(sys_state) {
    case ERROR_RFID:
    case ERROR_WEIGHT:
    case ERROR_INVALID:
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
    case READ_WEIGHT:
      read_weight();
      sys_state = OPEN_BARRIER;
      break;
    case OPEN_BARRIER:
      open_barrier();
      sys_state = SEND_DATA_OUT;
      break;
    case DENIED_ACCESS:
      show_access_denied();
      break;
    case SEND_DATA_OUT:
      send_card_data_to_master();
      break;
    case DATA_LINK:
      rf_protocol_manager();
      break;
  }
}


uint8_t read_rfid_value() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return 0;
  }
  for ( uint8_t i = 0; i < 4; i++) {  // 4 Byte UIDs
    readCard[i] = mfrc522.uid.uidByte[i];
  }
  mfrc522.PICC_HaltA(); // Stop reading
  return 1;
}

/**
 * Readed card must be checked agains the known ones
 */
bool check_card_and_act() {
  byte ret = is_known_card(readCard);
  if (ret) {
#ifdef WITH_SCALE
    sys_state = READ_WEIGHT;
#else 
    sys_state = OPEN_BARRIER;
#endif
    whos_entering = ret;
  } 
  return ret;
}

/**
 * Read the weight and store it in memory
 */
void read_weight() {
  char weight_data[7];
  String tmp;
  bool timeout = false;
  long start;
  start = millis();
  while(!Serial.available() && !timeout) {
    if (millis()-start > MAX_WEIGHT_WAIT_TIME) {
      timeout = true;
    }
  }
  if (Serial.available() && !timeout) {
    Serial.readBytes(weight_data, 7);
    for (byte i=0; i < 6; i++) {
      tmp += weight_data[i];
    }
    if (weight_data[6] == 0x1B || weight_data[6]==0x1C) {
      measured_weight = tmp.toInt();
    } else {
      measured_weight = -1;
    }
  } else {
    measured_weight = -1;
  }
}

void show_access_denied() {
  //TODO: Implement
}

bool is_known_card(byte card_id[4]) {
  //TODO: Implement
}

void open_barrier() {
  //TODO: Implement
}

void send_card_data_to_master() {
  //TODO: Implement
}