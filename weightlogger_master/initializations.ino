// Initialization code for the parts.

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
      }
    }
  }
}

void initialize_sd_card() {
  bool exit_init = false;
  uint8_t init_return;
  byte attemps = 0;
  while(!exit_init) {
    init_return = SD.begin(SDCARD_SS, SPI_HALF_SPEED);
    if (!init_return && attemps > 3) {
      sys_state = ERROR_SD;
      exit_init = true;
    } else {
      if (init_return) {
        exit_init = true;
      } else {
        delay(100); //wait 100 mSeconds
        attemps++;
      }
    }
  }
}

void initialize_rfid() {
  mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max); // Max reading distance
}
