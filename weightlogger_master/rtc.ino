#include "globals.h"

void initialize_rtc() {
  bool exit_init = false;
  uint8_t init_return;
  byte attemps = 0;
  while(!exit_init) {
    init_return = rtc.begin();
    if (!init_return && attemps > 2) {
      sys_state = ERROR_RTC;
      exit_init = true;
    } else {
      if (!init_return) {
        attemps ++;
      } else {
        exit_init = true;
        if (rtc.lostPower()) {
          // Fijar a fecha y hora de compilacion
          rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        }
      }
    }
  }
}

/**
 * Read the time and store it in memory
 */
bool read_rtc_value() {
  uint32_t tmp = 0;
  enteringTime = rtc.now();
  return true;
}

void adjust_rtc() {
  char dateString[32];
  char date[11];
  char time[9];
  char pos = 0;
  DateTime stamp;
  memset(dateString, 0, sizeof(dateString));
  memset(date, 0, sizeof(date));
  memset(time, 0, sizeof(time));
  Serial.write("M");
  Serial.flush();
  delay(1);
  pos = Serial.readBytesUntil("\r", dateString, sizeof(dateString));
  if (pos > 10) {
    stamp = split_date_time(dateString);
#ifdef DEBUG
    Serial.print("#");
    Serial.println(stamp.unixtime());
#endif
    rtc.adjust(stamp);
  }
  CLEAR_SERIAL
}

DateTime split_date_time(char *stamp) {
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