#ifndef _ELCAN_WIFI

#define INITIALIZATION_STR_LEN 5
#define IP_ADDRESS_LEN 15
#define WIFI_WAIT_TIMEOUT 450

class ElcanWifi {
  private:
    bool error;
    byte _error;
    void empty_serial_buffer();
  public:
    ElcanWifi();
    bool begin(long speed);
    int available();
    char poll();
    void get_ip(char *result);
    bool is_error();
    int write(char *data);
    void readCardData(char *result, char len);
};
#define _ELCAN_WIFI;
#endif
