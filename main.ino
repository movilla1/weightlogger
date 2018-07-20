#include <RTClib.h>
#include <SPI.h>
#include <SD.h>
#include <RFID.h>
#include <MFRC522.h>  // Library for Mifare RC522 Devices

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

uint8_t successRead;    // Variable integer to keep if we have Successful Read from Reader

byte storedCard[4];   // Stores an ID read from EEPROM
byte readCard[4];   // Stores scanned ID read from RFID Module


void setup() {
  Wire.begin();
  Serial.setup(4800);
  rtc.begin();
  sys_state = READY
  while(!exit_init) {
    init_return = SD.begin(chipSelect);
    if (!init_return && attemps > 3) {
      sys_state = ERROR_SD
      exit_init = true;
    } else {
      if (init_return) {
        exit_init = true;
      } else {
        sleep(5); //wait 5 seconds
        attemps++;
      }
    }
  }
  mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max); // Max reading distance

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
      if (read_rfid_value()) {
        check_card_and_act(); //checks the card and if its valid, it starts the sequence
      }
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
    case OPEN_BARRIER:
      open_barrier();
      sys_state = READY;
      break;
    default:
      usleep(10); //sleep while not doing anything
      break;
  }
}

uint8_t read_rfid_value() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return 0;
  }
  for ( uint8_t i = 0; i < 4; i++) {  // 4 Byte UIDs
    readCard[i] = mfrc522.uid.uidByte[i];
  }
  mfrc522.PICC_HaltA(); // Stop reading
  return 1;
}


void show_error(uint8_t error_code) {
  unit8_t blinks;
  switch(error_code) {
    case ERROR_SD:
      blinks = 2;
      break;
    case ERROR_RFID:
      blinks = 3;
      break;
    default:
      blinks = 1;
      break;
  }
  for (unit8_t i; i < blinks; i++) {
    digitalWrite(LED, 0); //turn off the LED
    usleep(300);
    digitalWrite(LED, 1)
    usleep(300);
  }
}

/**
 * Readed card must be checked agains the known ones
 */
// TODO: properly implement
bool check_card_and_act() {
  //do magic and return true if the card is valid, plus set the system status
  sys_state = READ_RTC;
  return true;
}

/**
 * Read the time and store it
 */
// TODO: properly implement
bool read_rtc_value() {
  return false;
}

/**
 * Read the weight and store it
 */
void read_weight() {
  // TODO: properly implement
}

void write_values_to_file() {
  // TODO: properly implement
}

void check_elapsed_time() {
  // TODO: implement it
}

void open_barrier() {
  // TODO: implement it
}