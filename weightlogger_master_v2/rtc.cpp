#include <Arduino.h>
#include "includes/rtc.h"
#include "includes/definitions.h"

ElcanRtc::ElcanRtc() {
}

bool ElcanRtc::initialize() {
  bool exit_init = false;
  unsigned char init_return;
  char attemps = 0;
  for (attemps = 0 ; attemps < 2; attemps++) {
    init_return = this->rtc.begin();
    if (init_return) {
      if (this->rtc.lostPower()) {
      // Fijar a fecha y hora de compilacion
        this->rtc.adjust(DateTime(FS(__DATE__), FS(__TIME__)));
      }
      break;
    }
  }
  if (attemps < 2)
    return true;
  else
    return false;
}

DateTime ElcanRtc::now() {
  return this->rtc.now();
}

void ElcanRtc::adjust_from_serial() {
  char dateString[32];
  char date[11];
  char time[9];
  char pos = 0;
  DateTime stamp;
  memset(dateString, 0, sizeof(dateString));
  memset(date, 0, sizeof(date));
  memset(time, 0, sizeof(time));
  Serial.write('M');
  Serial.flush();
  delay(1);
  pos = Serial.readBytesUntil('\r', dateString, sizeof(dateString));
  if (pos > 10) {
    stamp = split_date_time(dateString);
#ifdef DEBUG
    Serial.write('#');
    Serial.println(stamp.unixtime());
#endif
    this->rtc.adjust(stamp);
  }
  CLEAR_SERIAL
}

DateTime ElcanRtc::split_date_time(char *stamp) {
  const char chp[2] = "-";
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  char *section;
  section = strtok(stamp, chp);
  year = atoi(section);
  section = strtok(NULL, chp);
  month = atoi(section);
  section = strtok(NULL, chp);
  day = atoi(section);
  section = strtok(NULL, chp);
  hour = atoi(section);
  section = strtok(NULL, chp);
  min = atoi(section);
  section = strtok(NULL, chp);
  sec = atoi(section);
#ifdef DEBUG
  Serial.print("#");
  Serial.print(year);
  Serial.print("-");
  Serial.print(month);
  Serial.print("-");
  Serial.print(day);
  Serial.print("#");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(min);
  Serial.print(":");
  Serial.println(sec);
#endif
  return DateTime(year, month, day, hour, min, sec);
}
