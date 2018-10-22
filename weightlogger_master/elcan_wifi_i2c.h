#ifndef _ELCAN_WIFI_I2C

#define INITIALIZATION_STR_LEN 5
#define IP_ADDRESS_LEN 15

class ElcanWifiI2C {
  private:
    int _i2c_addr;
    byte _error;
  public:
    ElcanWifiI2C();
    bool begin(int addr);
    int available();
    char poll();
    void get_ip(char *result);
    bool is_error();
    int write(char *data);
    void readCardData(char *result, char len);
};
#define _ELCAN_WIFI_I2C;
#endif
