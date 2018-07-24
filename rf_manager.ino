/**
 * Radio initialization
 * 
 **/

void initialize_radio() {
  radio.begin();                           // Setup and configure rf radio
  radio.setChannel(1);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setAutoAck(1);                     // Ensure autoACK is enabled
  radio.setRetries(2,15);
  radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance
  radio.openReadingPipe(1,addresses[0]);      // Open a reading pipe on address 0, pipe 1
  radio.openReadingPipe(2,addresses[1]);      // Open a reading pipe on address 1, pipe 2
  radio.startListening();
  lastCommFrom = 0; //start with no lastComms
}

/**
 * Answers the RF link data/operations
 * Protocol v1.0
 * Sxy: replacing x for a byte coded number from 0 to 199,
 *            stores a card into the eeprom With card ID set to Y (Byte coded value from 0 to 255)
 * D: Dumps the eeprom memory in BYTE FORMAT
 * Bxxxxwwwwwwww: Slave Block, xxxx = Card ID (4 bytes) readed by the slave reader
 *                wwwwwww = 7 bytes weight
 * Ex: Erase card @ position x (x = byte from 0 to 199)
 * 
 * answers:
 * V: Card ID is ok
 * I: Card ID is not allowed.
 **/
void rf_protocol_manager() {
  char data_buffer[12];
  char tmp[7];
  byte ret;
  if (radio.available()) {
    radio.read(data_buffer, 1);
    
    switch(data_buffer[0]) {
      case 'S':
        radio.read(data_buffer,2);
        rf_store_card(data_buffer);
        break;
      case 'D':
        dump_eeprom();
        break;
      case 'B':
        radio.read(data_buffer,11);
        memcpy(readCard, data_buffer, 4);
        ret = is_known_card(readCard);
        answer_to_sender(ret);
        if (ret) { //valid card, store the data
          memcpy(tmp, data_buffer + 4, 7); //get the 7 remaining bytes for the weight
          measured_weight = get_weight_value(tmp);
          enteringTime = rtc.now();
          write_values_to_file();
          sys_state = READY;
        }
        break;
      case 'W':
        radio.read(data_buffer,7); //get all the 7 bytes for weight
        measured_weight = get_weight_value(data_buffer);
        break;
      case 'E':
        radio.read(data_buffer, 3);
        delete_card(data_buffer);
        break;
    }
  }
}

void rf_store_card(char card_data[]) {
  //TODO: Implement
}

void dump_eeprom() {
  //TODO: Implement
}

void answer_to_sender(bool opt) {
  byte ret;
  ret = opt ? VALID : INVALID;
  send_by_rf(ret);
}

void delete_card(char *card_data) {
  //TODO: Implement
}

uint16_t get_weight_value(char *weight_data) {
  //TODO: Implement
}

void send_by_rf(byte option) {
  radio.stopListening(); //start transmit mode
  radio.openWritingPipe(addresses[lastCommFrom]);    // writing to the ELCNM channel
  radio.write(&option, 1);
  radio.startListening();
}

void send_by_rf(char *data, uint8_t length) {
  radio.stopListening(); //start transmit mode
  radio.openWritingPipe(addresses[lastCommFrom]);    // writing to the ELCNM channel
  radio.write(&data, length);
  radio.startListening();
}