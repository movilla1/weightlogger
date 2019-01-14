#ifndef _ELCAN_SCALE

#define INITIALIZATION_STR_LEN 5

class ElcanScale {
  private:
    bool error;
    byte _error;
    uint8_t _i2c_address;

  protected:
    bool do_get_weight(byte *);

  public:
    ElcanScale(byte i2c_address);
    bool begin();
    void get_weight(byte *);
    bool is_error();
};
#define _ELCAN_SCALE;
#endif
