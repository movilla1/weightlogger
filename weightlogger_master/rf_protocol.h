#ifndef __ELCANPROTO__
#define __ELCANPROTO__ 1
/***
 * RF Protocol Constants
 * 
 **/
#define RADIO_PACKET_SIZE 24 //each packet will have 24 bytes, 2 for command, 1 for checksum, 21 data bytes
#define CSUM_POSITION 23
#define SERIAL_POSITION 2 // third byte is the serial number
#define MAX_PACKET_WAIT 160 // 160 mS to wait for a packet as maximum (assumes a slow connection of about 1200bps)
#define PACKET_PAYLOAD_SIZE 20 
#define PACKET_COMMAND_SIZE 2
#define MAX_PACKETS_PER_COMMAND 200

// Commands
#define NOP 0  // "NO"
#define VALID 1 // "VA"
#define ACK 2   // "AK"
#define NACK 4  // "NA"
#define TIME_ADJUST 8  // "TA"
#define STORE_CARD 16 // "SC"
#define DUMP_EEPROM 32   // "DU"
#define DUMP_SDCARD 40 // 'DC' Dump Card
#define SLAVE_BLOCK 64  // "SB"
#define ERASE_CARD 128  // "EC"
#define INVALID 255   // "IN"

/**
 * Every packet in this protocol has 24 bytes in length and is composed as shown below:
 * 
 * CCSddddd...ddX
 * 
 * Where:
 * CC = two bytes for the command identification
 * ddd..dd = up to 20 data bytes
 * S = serial for the packet (if retransmit then > 0)
 * X = checksum for the packet
 * 
 * E.G.:
 *      EC.+...................M 
 * (where . = 0x00) is the command to erase card @ position 43
 **/

struct elcanPacket {
  byte command[PACKET_COMMAND_SIZE];
  byte sequence;
  byte payload[PACKET_PAYLOAD_SIZE];
  byte checksum;
};

class ElcanProto
{
  RF24 *radio;
  struct elcanPacket data_packet;
  byte writeAddress[5];
  bool protocolError;
  byte max_retries;
  long startActionTime;

  const byte protocol_commands[9][2] = {
    {NACK, NOP}, {VALID, 0}, {ACK, 0}, {TIME_ADJUST, 0}, {STORE_CARD, SLAVE_BLOCK},
    {DUMP_EEPROM, DUMP_SDCARD}, {ERASE_CARD, 0}, {INVALID, 0}, {0, 0} };

  public:
    byte getPacket(uint8_t* pipe_num);
    void sendPacket();
    void fillPacket(byte command[], byte data[], uint8_t length);
    void fillPacket(const char command[], byte data[], uint8_t length);
    byte calculateChecksum(byte *data, byte len);
    void begin(byte writeAddress[], uint8_t max_retries, RF24 *radio);
    void resendLastPacket();
    bool getPacketPayload(byte*);
    void sendNack();
    ElcanProto();

  private:
    void clearPacket();
    bool verifyChecksum();
    void sendAck();
    byte parseCommand();
    void rfSendPacket();
};

#endif