/**
 * PC communication management
 */
void serial_manager() {
  char command;
  char params[20];
  command = Serial.read();
  
  switch (command)
  {
    case 'S':
      Serial.readBytes(params, 2); //card position and card identifier bytes
      rf_send_store_card(params);
      break;
    case 'A':
      Serial.readBytes(params, 19); //read the string date/time in YYYY-mm-dd hh:ii:ss format
      rf_send_adjust_time(params);
      break;
    case 'D':
      rf_send_dump_command();
      break;
    case 'E':
      Serial.readBytes(params,1);
      rf_send_delete_card(params);
    default:
      Serial.println("NAK");
      break;
  }
}

void rf_send_store_card(char *data) {
  send_rf_command('S', data, 2);
}

void rf_send_adjust_time(char *timestamp){
  send_rf_command('A', timestamp, 19);
}

void rf_send_dump_command(){
  char data = 0;
  send_rf_command('D', &data, 0);
}

void rf_send_delete_card(char *card) {
  char position = card[0];
  send_rf_command('E', &position, 1);
}

void send_rf_command(char command, char *data, byte len) {
  char send_str[21];
  memset(send_str,0, sizeof(send_str));
  send_str[0] = command;
  if (len > 0) {
    memcpy(send_str+1, data, len);
  }
  send_by_rf(send_str, len+1, MASTER_LOGGER);
}