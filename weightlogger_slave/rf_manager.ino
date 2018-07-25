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
  radio.openReadingPipe(1, addresses[0]); // Open a reading pipe on address 0, pipe 1
  radio.startListening();
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
 * V\n: Card ID is ok
 * I\n: Card ID is not allowed.
 **/
void rf_protocol_manager()
{
  char data_buffer[2];
  byte ret;
  if (radio.available())
  {
    radio.read(data_buffer, 2);

    switch (data_buffer[0])
    {
    case 'V':
      if (data_buffer[1] == '\n')
      {
        sys_state = OPEN_BARRIER;
      }
      break;
    case 'I':
      if (data_buffer[1] == '\n')
      {
        sys_state = DENIED_ACCESS;
      }
      break;
    }
  }
}

void answer_to_sender(bool opt)
{
  byte ret;
  ret = opt ? VALID : INVALID;
  send_by_rf(ret);
}

void send_by_rf(byte option)
{
  radio.stopListening();               //start transmit mode
  radio.openWritingPipe(addresses[0]); // writing to the ELCN1 channel
  radio.write(&option, 1);
  radio.startListening();
}

void send_by_rf(char *data, uint8_t length)
{
  radio.stopListening();               //start transmit mode
  radio.openWritingPipe(addresses[0]); // writing to the ELCN1 channel
  radio.write(&data, length);
  radio.startListening();
}
