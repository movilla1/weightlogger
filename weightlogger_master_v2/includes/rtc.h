#ifndef _ElcanRTC_
#include <RTClib.h>
#define _ElcanRTC_ 1
class ElcanRtc {
  public:
    ElcanRtc();
    bool initialize();
    long value();
    void adjust_from_serial();
    DateTime now();

  protected:
    RTC_DS3231 rtc;
    DateTime split_date_time(char *stamp);
};
#endif