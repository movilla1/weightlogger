/**
 * Answers the RF link data/operations
 * Protocol v1.0
 * Sxxxy: replacing xxx for a number from 0 to 200, 
 *            stores a card into the eeprom With card ID set to Y (Byte coded value from 0 to 255)
 * D: Dumps the eeprom memory in BYTE FORMAT
 * R: Card ID readed by the slave reader
 * W: Start weight measurement transfer (next 7 bytes are from the weight measurement device)
 * 01234567: Any 7 bytes can be loaded after the WEIGHT command is received.
 * Exxx: Erase card @ position xxx (0 to 200)
 * 
 * answers:
 * VALID: Card ID is ok
 * INVAL: Card ID is not allowed.
 **/
void answer_rf() {
  char data_buffer[7];
  char remaining[4];
  byte state = 1;
  if (radio.available()) {
    radio.read(data_buffer, 1);
    
    switch(data_buffer[0]) {
      case 'S':
        //rf_store_card();
        break;
      case 'D':
        //dump_eeprom();
        break;
      case 'R':
        //rf_check_card_and_answer();
        break;
      case 'W':
        //set_weight_read();
        break;
      case 'E':
        //delete_card(remaining);
        break;
      default:
        if (wait_weight) {
          //set the weight
        }
    }
  }
  //TODO: Implement protocol handshake
}