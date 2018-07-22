// Initialization code for the parts.

void initialize_radio() {
  radio.begin();                           // Setup and configure rf radio
  radio.setChannel(1);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setAutoAck(1);                     // Ensure autoACK is enabled
  radio.setRetries(2,15);
  radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance
}

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
    init_return = SD.begin(CHIP_SELECT_SD);
    if (!init_return && attemps > 3) {
      sys_state = ERROR_SD;
      exit_init = true;
    } else {
      if (init_return) {
        exit_init = true;
      } else {
        delay(5000); //wait 5 seconds
        attemps++;
      }
    }
  }
}
