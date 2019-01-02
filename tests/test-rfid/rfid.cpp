// Initialization code for the parts.
#include "includes/rfid.h"

RfidManager::RfidManager(MFRC522 *rfidObj) {
  memset(readCard, 0, sizeof(readCard));
  mfrc522 = rfidObj;
}

void RfidManager::begin() {
  SPI.begin();      // Init SPI bus
  reset();
}

void RfidManager::reset() {
  mfrc522->PCD_Init(); // Init MFRC522
  mfrc522->PCD_SetAntennaGain(MFRC522::RxGain_38dB);
#ifdef DEBUG
  mfrc522->PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
#endif
}
void RfidManager::debug() {
  mfrc522->PCD_DumpVersionToSerial();
  bool result = mfrc522->PCD_PerformSelfTest(); // perform the test
  Serial.println(F("#-----------------------------"));
  Serial.print(F("#Result: "));
  if (result)
    Serial.println(F("#OK"));
  else
    Serial.println(F("#DEFECT or UNKNOWN"));
}

byte RfidManager::getID() {
  if ( ! mfrc522->PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522->PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
  #ifdef DEBUG
    Serial.println("Failed serial #");
  #endif
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
