/**
 * Radio initialization
 * 
 **/
#include "rf_protocol.h"

void initialize_radio() {
  radio.begin();                           // Setup and configure rf radio
  radio.setChannel(1);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setAutoAck(1);                     // Ensure autoACK is enabled
  radio.setRetries(2,15);
  radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance
  radio.openReadingPipe(1,addresses[2]);      // Open a reading pipe on address 1, pipe 2
  radio.startListening();
  protocolManager.begin(addresses[1], 5, &radio);
}

void radio_comm_manager() {
  byte cmd;
  byte data[PACKET_PAYLOAD_SIZE];

  cmd = protocolManager.getPacket();
  switch (cmd) {
    case VALID:
      Serial.println("Valid Data Received");
      break;
    case INVALID:
      Serial.println("Invalid Data");
      break;
    case ACK:
      Serial.println("Command OK");
      attemps = 0;
      break;
    case NACK:
      if (attemps < 5) {
        protocolManager.resendLastPacket();
        attemps += 1;
      } else {
        Serial.println("Error receiving data");
      }
      break;
    case DUMP:
      protocolManager.getPacketPayload(data);
      for(byte i=0; i < PACKET_PAYLOAD_SIZE; i++) {
        Serial.write(data[i]);
      }
      break;
  }
}