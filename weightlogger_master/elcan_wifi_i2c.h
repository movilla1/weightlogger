#ifndef _ELCAN_WIFI_I2C

#define INITIALIZATION_STR_LEN 5
#define IP_ADDRESS_LEN 15

class ElcanWifiI2C {
  private:
    byte _i2c_addr;
    byte _error;
  public:
    ElcanWifiI2C();
    void begin(byte i2c_addr);
    String get_ip_address();
    bool is_error();
    void set_server_ip(char *ip);
    int write(String);
};
#define _ELCAN_WIFI_I2C;
#endif