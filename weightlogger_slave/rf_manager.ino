/**
 * Radio initialization
 * 
 **/
#include "rf_protocol.h"

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
  protocol_manager.begin(addresses[1], 5, &radio);
}

/**
 * Answers the RF link data/operations
 * Protocol v1.0
 * ST x y: replacing x for a byte coded number from 0 to 199,
 *            stores a card into the eeprom With card ID set to Y (Byte coded value from 0 to 255)
 * DU: Dumps the eeprom memory in BYTE FORMAT
 * SB xxxx wwwwwwww: Slave Block, xxxx = Card ID (4 bytes) readed by the slave reader
 *                wwwwwww = 7 bytes weight
 * AT yyyy-mm-dd hh:ii:ss : Adjust time, YYYY=year 4 digits, mm=month 00-12, dd=day 01-31, 
 *                hh=hour 00-23, ii=minutes 00-59, ss=seconds 00-59
 * EC x: Erase card @ position x (x = byte from 0 to 199)
 * 
 * answers:
 * VA: Card ID is ok
 * IN: Card ID is not allowed.
 **/
void rf_protocol_manager()
{
  char data_buffer[24];
  byte command = NOP;
  byte ret;
  if (radio.available())
  {
    command = protocol_manager.getPacket();

    switch (command)
    {
    case VALID:
      sys_state = OPEN_BARRIER;    
      break;
    case INVALID:
      sys_state = DENIED_ACCESS;
      break;
    case NACK:
      protocol_manager.resendLastPacket();
      break;
    }
  }
}

void answer_to_sender(bool opt)
{
  byte tmp=0;
  if (opt) {
    protocol_manager.fillPacket("VA", &tmp, 0);
    protocol_manager.sendPacket();
  } else {
    protocol_manager.fillPacket("IN", &tmp, 0);
    protocol_manager.sendPacket();
  }
}
