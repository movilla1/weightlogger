/**
* Taken out as the serial port is now used.
*
#ifdef DEBUG
void serialOptions() {
  byte readed[2];
  byte cmd;
  byte buff[16];
  byte count;

  cmd = Serial.read();
  Serial.println("ACK");
  switch(cmd) {
    case 'S':
      Serial.readBytes(readed,2);
      Serial.println("Scan");
      debug_store_card(readed);
      Serial.println("Ready");
      break;
    case 'D':
      for (uint16_t i = 0; i < 0x400; i++)
      {
        buff[0] = EEPROM.read(i);
        Serial.print(buff[0],HEX);
        Serial.print(" ");
        if (i % 20 == 0) {
          Serial.println(" |");
        }
      }
      Serial.println("Dump finished");
      break;
  }
}

void debug_store_card(byte *card_data) {
  bool finish = false;
  bool timeout = false;
  long tstart;
  tstart = millis();
  struct card_block card;
  while (!finish || timeout)
  {
    if (millis() - tstart > MAX_CARD_WAIT_TIME)
    {
      timeout = true;
    }
    if (getID()) {
      card.card_number = card_data[1];
      memcpy(card.card_uid, readCard, 4);
      store_card(card, card_data[0]);
      finish = true;
    }
  }
}
#endif
*/