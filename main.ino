#include <RTClib.h>
#include <SPI.h>
#include <SD.h>
#include <RFID.h>
#include <MFRC522Debug.h>
#include <deprecated.h>
#include <require_cpp11.h>
#include <MFRC522Hack.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>


/**
 * SD card attached to SPI bus as follows:
 * MOSI - pin 11
 * MISO - pin 12
 * CLK - pin 13
 * CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
 */

#define READY 1
#define ERROR_SD 2
#define ERROR_RFID 4
#define READ_RFID 8
#define READ_RTC 16
#define READ_WEIGHT 32
#define OPEN_BARRIER 64
#define WRITE_RECORD 128
#define TIMED_WAIT 256

#define BARRERA 0
#define SD_CS 1
#define RTC_CS 2


File myFile;

int sys_state;

void setup() {
  Wire.begin();
  Serial.setup(4800);
  rtc.begin();
  sys_state = READY
  if (!SD.begin(chipSelect)) {
    sys_state = ERROR_SD
  }
  myFile = SD.open("datalog.csv", FILE_WRITE);
}

void loop() {
  switch(sys_state) {
    case ERROR_SD:
      show_error(ERROR_SD);
      break;
    case ERROR_RFID:
      show_error(ERROR_RFID);
      break;
    case READ_RFID:
      read_rfid_value();
      sys_state = READ_RTC;
      break;
    case READ_RTC:
      read_rtc_value();
      sys_state = READ_WEIGHT;
      break;
    case READ_WEIGHT:
      read_weight();
      sys_state = WRITE_RECORD;
      break;
    case WRITE_RECORD:
      write_values_to_file();
      sys_state = TIMED_WAIT;
      break;
    case TIMED_WAIT:
      if (time_finished) {
        sys_state = OPEN_BARRIER;
      } else {
        check_elapsed_time();
      }
      break;
    default:
      usleep(10); //sleep while not doing anything
  }
}

