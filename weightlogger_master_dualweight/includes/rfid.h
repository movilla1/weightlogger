#ifndef __RFID_
//#define DEBUG 1
#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>

#define RFID_RST  9    //Pin 9 para el reset del RC522
#define RFID_SS   10  //Pin 10 para el SS (SDA) del RC522

class RfidManager {
  protected:
    MFRC522 *mfrc522;
    
  public:
    byte readCard[4];
    RfidManager(MFRC522 *rfidObj);
    void begin();
    byte getID();
    void debug();
    void reset();
};

#define __RFID_ 1
#endif
