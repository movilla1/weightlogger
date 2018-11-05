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
  bool valid_stamp = false;
  DateTime dt_adj;
  memset(dateString, 0, sizeof(dateString));
  while (Serial.available()) {
    tmp = Serial.read();
    if (tmp!='\r') {
      dateString[pos] = tmp
      pos ++;
      pos %= sizeof(dateString);
    } else {
      valid_stamp = true;
      break;
    }
  }
  if (valid_stamp) {
    split_date_time(dateString, date, time)
    dt_adj = new DateTime(date, time);
    rtc.adjust(dt_adj);
  }
  empty_serial_buffer();
}

void split_date_time(char *stamp, char *date, char *time) {
  
}