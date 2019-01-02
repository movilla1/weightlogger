#include <SPI.h>
#include "includes/rfid.h"

MFRC522 rfidObj(RFID_SS, RFID_RST);
RfidManager rfid(&rfidObj);
char id;

void setup() {
  SPI.begin();
  rfid.begin();
  Serial.begin(9600);
  Serial.println("Init done!");
  rfid.debug();
  rfid.reset();
}

void loop() {
  if (rfid.getID()) {
    Serial.println("#Readed Card");
    for(char x=0; x<4;x++){
      Serial.print(rfid.readCard[x], HEX);
      Serial.print(" ");
    }
    Serial.println(" ");
  }
  delay(800);
}
