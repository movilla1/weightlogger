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
    String get_ip();
    bool is_error();
    void set_server_ip(char *ip);
    int write(String data);
};
#define _ELCAN_WIFI_I2C;
#endif