// Initialization code for the parts.
#include "includes/rfid.h"

MFRC522 rfidObj(RFID_SS, RFID_RST);

RfidManager::RfidManager() {
  memset(readCard, 0, sizeof(readCard));
  mfrc522 = &rfidObj;
}

void RfidManager::begin() {
  mfrc522->PCD_Init();    // Initialize MFRC522 Hardware
  mfrc522->PCD_SetAntennaGain(mfrc522->RxGain_max); // Max reading distance
}

unsigned char RfidManager::getID() {
  if ( ! mfrc522->PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522->PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return 0;
  }
  // every PICC as they have 4 byte UID
#ifdef DEBUG
  Serial.print("#");
#endif
  for ( uint8_t i = 0; i < 4; i++) {  //
    readCard[i] = mfrc522->uid.uidByte[i];
#ifdef DEBUG
    Serial.print(readCard[i], HEX);
    Serial.print("-");
#endif
  }
#ifdef DEBUG
    Serial.println(" ");
#endif
  mfrc522->PICC_HaltA(); // Stop reading
  return 1;
}
