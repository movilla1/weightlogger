#ifndef __RFID_
#include <MFRC522.h>

#define RFID_RST  9    //Pin 9 para el reset del RC522
#define RFID_SS   10  //Pin 10 para el SS (SDA) del RC522

class RfidManager {
  protected:
    MFRC522 mfrc522; //Creamos el objeto para el RC522
    
  public:
    char readCard[4];
    RfidManager();
    void begin();
    unsigned char getID();
};

#define __RFID_ 1
#endif