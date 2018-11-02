#ifndef _ELCAN_WIFI

#define INITIALIZATION_STR_LEN 5
#define IP_ADDRESS_LEN 15
#define WIFI_WAIT_TIMEOUT 450
#define INTRUSSION_PACKET_SIZE 8
#define ENTRY_PACKET_SIZE 12
#define CARD_PACKET_SIZE 6
#define EOL "\r\n"

class ElcanWifi {
  private:
    bool error;
    byte _error;
    void empty_serial_buffer();
  public:
    ElcanWifi();
    bool begin();
    int available();
    char poll();
    void get_ip(char *result);
    bool is_error();
    byte sendEntry(char *data);
    void sendIntrussionAttemp(char *data);
    void readCardData(char *result, char len);
};
#define _ELCAN_WIFI;
#endif
