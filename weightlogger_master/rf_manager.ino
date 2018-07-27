/**
 * Radio initialization
 * 
 **/

void initialize_radio()
{
  radio.begin(); // Setup and configure rf radio
  radio.setChannel(1);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setAutoAck(1); // Ensure autoACK is enabled
  radio.setRetries(2, 15);
  radio.setCRCLength(RF24_CRC_8);         // Use 8-bit CRC for performance
  radio.openReadingPipe(1, addresses[1]); // Open a reading pipe on address 0, pipe 1
  radio.openReadingPipe(2, addresses[2]); // Open a reading pipe on address 1, pipe 2
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
 * Ayyyy-mm-dd hh:ii:ss : Adjust time, YYYY=year 4 digits, mm=month 00-12, dd=day 01-31, 
 *                hh=hour 00-23, ii=minutes 00-59, ss=seconds 00-59
 * Ex: Erase card @ position x (x = byte from 0 to 199)
 * 
 * answers:
 * V: Card ID is ok
 * I: Card ID is not allowed.
 **/
void rf_protocol_manager()
{
  char data_buffer[20];
  char tmp[7];
  byte ret;
  if (radio.available())
  {
    radio.read(data_buffer, 1);

    switch (data_buffer[0])
    {
    case 'A':
      radio.read(data_buffer, 19); //time is 19 characters long; (e.g.: 2018-07-25 12:03:10)
      adjust_time(data_buffer);
      break;
    case 'S':
      radio.read(data_buffer, 2);
      rf_store_card(data_buffer);
      break;
    case 'D':
      dump_eeprom();
      break;
    case 'B':
      radio.read(data_buffer, 11);
      memcpy(readCard, data_buffer, 4);
      ret = is_known_card(readCard);
      answer_to_sender(ret);
      if (ret)
      {                                  //valid card, store the data
        memcpy(tmp, data_buffer + 4, 7); //get the 7 remaining bytes for the weight
        measuredWeight = get_weight_value(tmp);
        enteringTime = rtc.now();
        write_values_to_file('I');
        sys_state = READY;
      }
      break;
    case 'W':
      radio.read(data_buffer, 7); //get all the 7 bytes for weight
      measuredWeight = get_weight_value(data_buffer);
      break;
    case 'E':
      radio.read(data_buffer, 3);
      delete_card(data_buffer);
      break;
    }
  }
}

void rf_store_card(char *card_data)
{
  bool finish = false;
  bool timeout = false;
  int tstart;
  tstart = millis();
  struct card_block card;
  while (!finish || timeout)
  {
    if (millis() - tstart > MAX_CARD_WAIT_TIME)
    {
      timeout = true;
    }
    if (mfrc522.PICC_IsNewCardPresent())
    {
      read_rfid_value();
      card.card_number = card_data[1];
      memcpy(card.card_uid, readCard, 4);
      EEPROM_writeBlock(card_data[0], card);
      finish = true;
    }
  }
}

void dump_eeprom()
{
  //Dump the whole eeprom
  char buff[16];
  byte pos = 0;
  for (int i = 0; i < 0x400; i++)
  {
    buff[pos] = EEPROM.read(i);
    pos++;
    if (pos >= sizeof(buff))
    {
      pos = 0;
      send_by_rf(buff, sizeof(buff));
    }
  }
}

void answer_to_sender(bool opt)
{
  byte ret;
  ret = opt ? VALID : INVALID;
  send_by_rf(ret);
}

void delete_card(char *card_data)
{
  struct card_block card;
  memset(card.card_uid, 0xff, sizeof(card.card_uid));
  card.card_number = 255;
  EEPROM_writeBlock(card_data[0], card); //writing everythign to 255 clears the eeprom block.
}

uint16_t get_weight_value(char *weight_data)
{
  String str;
  uint16_t weight;
  if (weight_data[6] == 0x1B || weight_data[6] == 0x1C)
  {
    for (byte i = 0; i < 6; i++)
    {
      str += (char)weight_data[i];
    }
    weight = str.toInt();
  }
  return weight;
}

void send_by_rf(byte option)
{
  byte tmp[2];
  tmp[0] = option;
  tmp[1] = '\n';
  radio.stopListening();               //start transmit mode
  radio.openWritingPipe(addresses[0]); // writing to the ELCN1 channel
  radio.write(tmp, 2);
  radio.startListening();
}

void send_by_rf(char *data, uint8_t length)
{
  radio.stopListening();               //start transmit mode
  radio.openWritingPipe(addresses[0]); // writing to the ELCN1 channel
  radio.write(&data, length);
  radio.startListening();
}

void adjust_time(char *data)
{
  rtc.adjust(DateTime(data));
}