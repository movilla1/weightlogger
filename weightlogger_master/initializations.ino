// Initialization code for the parts.
#include "globals.h"

void initialize_rtc() {
  bool exit_init = false;
  uint8_t init_return;
  byte attemps = 0;
  while(!exit_init) {
    init_return = rtc.begin();
    if (!init_return && attemps > 2) {
      sys_state = ERROR_RTC;
      exit_init = true;
    } else {
      if (!init_return) {
        attemps ++;
      } else {
        exit_init = true;
        if (rtc.lostPower()) {
          // Fijar a fecha y hora de compilacion
          rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        }
      }
    }
  }
}

void initialize_rfid() {
  mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max); // Max reading distance
}

void initialize_wifi() {
  char tmp;
  /*while(wifi.available()) { //read all the bytes at startup ()
    tmp = wifi.read();
  }*/
}