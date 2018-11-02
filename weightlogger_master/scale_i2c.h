#ifndef _ELCAN_SCALE

#define INITIALIZATION_STR_LEN 5

class ElcanScale {
  private:
    bool error;
    byte _error;
    uint8_t _i2c_address;

  public:
    ElcanScale();
    bool begin(uint8_t i2c_address);
    uint16_t get_weight();
    bool is_error();
};
#define _ELCAN_SCALE;
#endif
