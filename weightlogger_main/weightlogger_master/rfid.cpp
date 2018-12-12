// Initialization code for the parts.
#include "rfid.h"

RfidManager::RfidManager() {
  memset(this->readCard, 0, sizeof(this->readCard));
  this->mfrc522 = MFRC522(RFID_SS, RFID_RST);
}

void RfidManager::begin() {
  this->mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
  this->mfrc522.PCD_SetAntennaGain(this->mfrc522.RxGain_max); // Max reading distance
}

unsigned char RfidManager::getID() {
  if ( ! this->mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! this->mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return 0;
  }
  // every PICC as they have 4 byte UID
#ifdef DEBUG
  Serial.print("#");
#endif
  for ( uint8_t i = 0; i < 4; i++) {  //
    this->readCard[i] = this->mfrc522.uid.uidByte[i];
#ifdef DEBUG
    Serial.print(readCard[i], HEX);
    Serial.print("-");
#endif
  }
#ifdef DEBUG
    Serial.println(" ");
#endif
  this->mfrc522.PICC_HaltA(); // Stop reading
  return 1;
}

