/**
 * PC communication management
 */
void serial_manager() {
  byte command;
  byte params[20];
  if (Serial.available()) {
    command = Serial.read();
    switch (command)
    {
      case 'S':
        Serial.readBytes(params, 2); //card position and card identifier bytes
        rf_send_store_card(params);
        Serial.println("ACK");
        break;
      case 'A':
        Serial.readBytes(params, 19); //read the string date/time in YYYY-mm-dd hh:ii:ss format
        rf_send_adjust_time(params);
        Serial.println("ACK");
        break;
      case 'D':
        rf_send_dump_command();
        Serial.println("ACK");
        break;
      case 'C':
        rf_send_dump_sd();
        Serial.println("ACK");
        break;
      case 'E':
        Serial.readBytes(params,1);
        rf_send_delete_card(params);
        Serial.println("ACK");
        break;
      default:
        Serial.println("NAK");
        break;
    }
  }
}

void rf_send_store_card(byte *data) {
  send_rf_command("SC", data, 2);
}

void rf_send_adjust_time(byte *timestamp){
  send_rf_command("AT", timestamp, 19);
}

void rf_send_dump_command(){
  byte data = 0;
  send_rf_command("DU", &data, 0);
}

void rf_send_dump_sd() {
  byte data = 0;
  send_rf_command("DC", &data, 0);
}

void rf_send_delete_card(byte *card) {
  byte position = card[0];
  send_rf_command("EC", &position, 1);
}

void send_rf_command(const char command[], byte *data, byte len) {
  protocolManager.fillPacket(command, data, len);
  protocolManager.sendPacket();
}
