#include <RTClib.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <MFRC522.h>  // Library for Mifare RC522 Devices
#include <RF24.h>

#include "eepromblock.h"
#include "definitions.h"

struct card_block {
  byte card_uid[4];
  byte card_number;
};

/**
 * SD card attached to SPI bus as follows:
 * MOSI - pin 11
 * MISO - pin 12
 * CLK - pin 13
 * CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
 */


/**
 * Global Variables
 */
File myFile;
DS3231 rtc;
MFRC522 mfrc522;
byte sys_state;
byte readCard[4];   // Stores scanned ID read from RFID Module
byte whos_entering; //stores in ram the card position that's readed
uint16_t measuredWeight; // Stores weight in ram
DateTime enteringTime;  //last time readed on the RTC
DateTime timerStarted;
ElcanProto protocolManager;
/* Radio related config */
byte addresses[][6] = {"ELCN1","ELCN2","ELCNM"};
RF24 radio(7, 8); //RF24 Radio on pins 7 & 8
byte lastCommFrom;
/**
 * System setup
 */
void setup() {
  Wire.begin();
  pinMode(LED, OUTPUT);
  pinMode(BARRERA, OUTPUT);
  digitalWrite(BARRERA, 0);
  digitalWrite(LED, 0);
  Serial.begin(4800, SERIAL_8N1); // according to wheight measurement device
  initialize_rtc();
  initialize_sd_card();
  initialize_radio();
  myFile = SD.open("datalog.csv", O_READ | O_WRITE | O_CREAT | O_APPEND);
  mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max); // Max reading distance
  sys_state = READY;
}

void loop() {
  switch(sys_state) {
    case ERROR_SD:
    case ERROR_RFID:
    case ERROR_RTC:
      show_error(sys_state);
      break;
    case READY:
      if (mfrc522.PICC_IsNewCardPresent()) {
        sys_state = READ_RFID;
      }
      if (radio.available(&lastCommFrom)){
        sys_state = DATA_LINK;
      }
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
      write_values_to_file('O');
      timerStarted = rtc.now();
      sys_state = TIMED_WAIT;
      break;
    case TIMED_WAIT:
      if (check_elapsed_time()) {
        sys_state = OPEN_BARRIER;
      }
      break;
    case OPEN_BARRIER:
      open_barrier();
      sys_state = READY;
      break;
    case DATA_LINK:
      rf_protocol_manager();
      break;
    default:
      delay(10); //sleep while not doing anything
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

/**
 * Readed card must be checked agains the known ones
 */
bool check_card_and_act() {
  byte ret = is_known_card(readCard);
  if (ret) {
    sys_state = READ_RTC;
    whos_entering = ret;
  } 
  return ret;
}

/**
 * Read the time and store it in memory
 */
bool read_rtc_value() {
  uint32_t tmp = 0;
  if (rtc.isrunning()) {
    enteringTime = rtc.now();
  } else {
    enteringTime = DateTime(tmp);
    sys_state = ERROR_RTC;
    return false;
  }
  return true;
}

/**
 * Read the weight and store it in memory
 */
void read_weight() {
  char weight_data[7];
  String tmp;
  bool timeout = false;
  long start;
  start = millis();
  while(!Serial.available() && !timeout) {
    if (millis()-start > MAX_WEIGHT_WAIT_TIME) {
      timeout = true;
    }
  }
  if (Serial.available() && !timeout) {
    Serial.readBytes(weight_data, 7);
    for (byte i=0; i < 6; i++) {
      tmp += weight_data[i];
    }
    if (weight_data[6] == 0x1B || weight_data[6]==0x1C) {
      measuredWeight = tmp.toInt();
    } else {
      measuredWeight = -1;
    }
  } else {
    measuredWeight = -1;
  }
}

void write_values_to_file(char action) {
  write_timestamp(enteringTime);
  myFile.write(";");
  myFile.write(whos_entering);
  myFile.write(";");
  myFile.write(measuredWeight);
  myFile.write(";");
  myFile.write(action);
  myFile.write(0x0D);
  myFile.write(0x0A);
}

void write_timestamp(DateTime stamp) {
   myFile.print(stamp.year(), DEC);
   myFile.print('/');
   myFile.print(stamp.month(), DEC);
   myFile.print('/');
   myFile.print(stamp.day(), DEC);
   myFile.print(" ");
   myFile.print(stamp.hour(), DEC);
   myFile.print(':');
   myFile.print(stamp.minute(), DEC);
   myFile.print(':');
   myFile.print(stamp.second(), DEC);
   myFile.print(" ");
}

void open_barrier() {
  digitalWrite(BARRERA, 1);
  delay(8000); //wait until the barrier acknowledges the open command
  digitalWrite(BARRERA, 0); //release Barrier switch
}

bool compare_card(byte card_id[4], struct card_block card) {
  bool ret_val = true;
  for (uint8_t i; i < 4; i++) {
    ret_val &= (card_id[i] == card.card_uid[i]);
  }
  return ret_val;
}

bool is_known_card(byte card_id[4]) {
  struct card_block card;
  bool finish = false;
  int pos = 1; // Pos 0 is for the last written card number
  byte ret_val = 0;
  while(!finish) {
    EEPROM_readBlock(pos, card);
    if (compare_card(card_id, card)) {
      finish = true;
      ret_val = card.card_number;
    }
    if (pos > MAX_EEPROM_POSITION) {
      finish = true;
      ret_val = false;
    }
  }
  return ret_val;
}

/**
 * Stores the card uid and card_number at the position indicated
 * positions are from 0 to 199
 */
void store_card(struct card_block card, byte position) {
  int pos = 5 * (position + 1);
  if ( pos < 200) {  //if we are not full capacity
    EEPROM_writeBlock(pos, card); //store the card
  } else {
    send_by_rf(INVALID);
  }
}

void start_time_measurement() {
  timerStarted = rtc.now();
}

bool check_elapsed_time() {
  DateTime current = rtc.now();
  if ((current - timerStarted).totalseconds() >= WAITING_TIME) {
    return true;
  }
  return false;
}
