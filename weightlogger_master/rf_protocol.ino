#include "rf_protocol.h"
#include <Arduino.h>

ElcanProto::ElcanProto() {
  this->protocolError = false;
}

void ElcanProto::begin(byte *wrAddress, uint8_t maxRetries, RF24 *radio) {
  memcpy(this->writeAddress, wrAddress, 5);
  this->max_retries = maxRetries;
  this->protocolError = false;
  this->radio = radio;
  clearPacket();
}

byte ElcanProto::getPacket(uint8_t* pipe_num)
{
  byte buffer[RADIO_PACKET_SIZE];
  byte count = 0;
  bool finish = false, shouldTimeout = false;
  byte command = INVALID;
  this->startActionTime = millis();
  while (!finish)
  {
    if (this->radio->available(pipe_num) && count < RADIO_PACKET_SIZE && !finish)
    {
      this->radio->read(&buffer[count], 1);
      count++;
    }
    else
    {
      if (count == RADIO_PACKET_SIZE)
      {
        memcpy(&this->data_packet, buffer, RADIO_PACKET_SIZE); //transfer to the packet data structure
        if (this->verifyChecksum())
        {
          this->sendAck();
          command = this->parseCommand();
          finish = true;
        }
        else
        {
          this->sendNack();
          finish = true;
        }
      } else {
        shouldTimeout = (startActionTime - millis()) > MAX_PACKET_WAIT;
        if (shouldTimeout) {
          this->sendNack();
          finish = true;
          this->protocolError = true;
        }
      }
    }
  }
  return command;
}

bool ElcanProto::getPacketPayload(byte data[]) {
  memcpy(data, this->data_packet.payload, PACKET_PAYLOAD_SIZE);
}

void ElcanProto::sendAck()
{
  this->clearPacket();
  this->data_packet.command[0]='A';
  this->data_packet.command[1]='K';
  this->data_packet.sequence = 0;
  this->sendPacket();
}

void ElcanProto::clearPacket() {
  memset(&this->data_packet, 0, sizeof(this->data_packet));
}

void ElcanProto::sendNack()
{
  this->clearPacket();
  this->data_packet.command[0]='N';
  this->data_packet.command[1]='A';
  this->sendPacket();
}

byte ElcanProto::calculateChecksum(byte *data, byte len)
{
  byte csum = 0;
  for (byte i = 0; i < len; i++)
  {
    csum += (~((byte)data[i] - 1));
  }
  return csum;
}

bool ElcanProto::verifyChecksum()
{
  byte sent_csum;
  byte calc_csum;
  bool ret = false;
  byte data[23];
  memcpy(data, &this->data_packet, 23); 
  calc_csum = this->calculateChecksum(data, 23);
  sent_csum = this->data_packet.checksum;
  if (calc_csum == sent_csum)
  {
    ret = true;
  }
  return ret;
}

void ElcanProto::fillPacket(byte command[], byte data[], uint8_t length) { 
  if (length > (RADIO_PACKET_SIZE - 4))
  { //can't send more than 20 bytes at once
    return;
  }
  this->clearPacket(); //get a fresh/clean packet, also sets sequence to 0
  memcpy(this->data_packet.command, command, 2);
  memcpy(this->data_packet.payload, data, length);
}
void ElcanProto::fillPacket(const char command[], byte data[], uint8_t length) { 
  if (length > (RADIO_PACKET_SIZE - 4))
  { //can't send more than 20 bytes at once
    return;
  }
  this->clearPacket(); //get a fresh/clean packet, also sets sequence to 0
  memcpy(this->data_packet.command, (byte *)command, 2);
  memcpy(this->data_packet.payload, data, length);
}

void ElcanProto::sendPacket()
{
  byte tmp[RADIO_PACKET_SIZE];
  memcpy(tmp, &this->data_packet, 23);
  this->data_packet.checksum = this->calculateChecksum(tmp, 23);
  this->rfSendPacket();
}

void ElcanProto::resendLastPacket() {
  this->data_packet.sequence += 1;
  if (data_packet.sequence <= MAX_PACKETS_PER_COMMAND) {
    this->rfSendPacket();
  } else {
    this->protocolError = true;
  }
}

void ElcanProto::rfSendPacket() {
  this->radio->stopListening();                //start transmit mode
  this->radio->openWritingPipe(this->writeAddress); // writing to the ELCN2 channel
  this->radio->write(&data_packet, RADIO_PACKET_SIZE);
  this->radio->startListening();
}

byte ElcanProto::parseCommand()
{
  const byte command_first[] = {
      'N', 'V', 'A', 'T',
      'S', 'D', 'E', 'I'};

  const byte command_second[8][2] = {
    {'A', 'O'}, // NO / NA
    { 'A', '\0' },
    { 'K', '\0' },
    { 'A', '\0' },
    { 'C', 'B' }, // SC / SB
    { 'U', 'C' }, // DU / DC
    { 'C', '\0' },
    { 'N', '\0' }
  };

  byte first, second;
  byte x_position = 8;
  byte y_position = 0;

  first = this->data_packet.command[0];
  second = this->data_packet.command[1];

  for (byte c = 0; c < sizeof(command_first); c++)
  {
    if (first == command_first[c])
    {
      x_position = c;
      break;
    }
  }
  if (x_position != 255)
  {
    for (byte d = 0; d < sizeof(command_first); d++)
    {
      if (second == command_second[d][0])
      {
        y_position = d;
      }
      if (second == command_second[d][1])
      {
        y_position = d + 1;
      }
    }
  }
  return this->protocol_commands[x_position][y_position];
}