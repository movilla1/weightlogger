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
RF24 radio(7,8); //RF24 Radio on pins 7 & 8
int sys_state;
byte successRead;    // Variable integer to keep if we have Successful Read from Reader
byte storedCard[4];   // Stores an ID read from EEPROM
byte readCard[4];   // Stores scanned ID read from RFID Module
byte whos_entering; //stores in ram the card position that's readed
byte whos_leaving; //stores in ram the card position that's readed
uint16_t measured_weight; // Stores weight in ram
DateTime enteringTime;  //last time readed on the RTC
DateTime leavingTime;  //last time readed on the RTC
DateTime timerStarted;
byte addresses[][6] = {"Node1","Node2","MASTR"};
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
  sys_state = READY;
  initialize_sd_card();
  initialize_radio();
  myFile = SD.open("datalog.csv", O_READ | O_WRITE | O_CREAT | O_APPEND);
  mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max); // Max reading distance
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
      if (radio.available()){
        sys_state = DATA_LINK;
      }
    case READ_RFID:
      if (read_rfid_value()) {
        check_card_and_act(ENTERING); //checks the card and if its valid, it starts the sequence
      }
      break;
    case READ_RTC:
      read_rtc_value(ENTERING);
      sys_state = READ_WEIGHT;
      break;
    case READ_WEIGHT:
      read_weight();
      sys_state = READ_RFID_2;
      break;
    case READ_RFID_2:
      if (read_rfid_value()) {
        check_card_and_act(LEAVING);
      }
      break;
    case READ_RTC_2:
      read_rtc_value(LEAVING);
      break;
    case WRITE_RECORD:
      write_values_to_file();
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
      answer_rf();
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


void show_error(uint8_t error_code) {
  uint8_t blinks;
  switch(error_code) {
    case ERROR_SD:
      blinks = 2;
      break;
    case ERROR_RFID:
      blinks = 3;
      break;
    case ERROR_RTC:
      blinks = 4;
    default:
      blinks = 1;
      break;
  }
  for (uint8_t i; i < blinks; i++) {
    digitalWrite(LED, 0); //turn off the LED
    delay(300);
    digitalWrite(LED, 1);
    delay(300);
  }
  delay(500); //wait 1/2 second between displays at least;
}

/**
 * Readed card must be checked agains the known ones
 */
bool check_card_and_act(byte card_slot) {
  byte ret = is_known_card(readCard);
  if (ret && card_slot == 1) {
    sys_state = READ_RTC;
    whos_entering = ret;
  } else {
    if (ret && card_slot == 2) {
      whos_leaving = ret;
      sys_state = READ_RTC_2;
    } else {
      sys_state = ERROR_INVALID;
    }
  }
  return ret;
}

/**
 * Read the time and store it in memory
 */
bool read_rtc_value(byte action) {
  switch (action) {
    case ENTERING:
      enteringTime = rtc.now();
      break;
    case LEAVING:
      leavingTime = rtc.now();
      break;
  }
  return true;
}

/**
 * Read the weight and store it in memory
 */
void read_weight() {
  // TODO: properly implement
  measured_weight = 50;
}

void write_values_to_file() {
  char s_date[20];
  sprintf(s_date, "%s", enteringTime.format("Y-m-d h:m:s"));
  myFile.write(s_date);
  myFile.write(";");
  myFile.write(whos_entering);
  myFile.write(";");
  myFile.write(measured_weight);
  myFile.write(";");
  sprintf(s_date, "%s", leavingTime.format("Y-m-d h:m:s"));
  myFile.write(s_date);
  myFile.write(";");
  myFile.write(whos_leaving);
  myFile.write(0x0D);
  myFile.write(0x0A);
}

void open_barrier() {
  digitalWrite(BARRERA, 1);
  delay(8000); //wait until the barrier acknowledges the open command
  digitalWrite(BARRERA, 0); //release Barrier switch
}

bool compare_card(byte card_id[4], struct card_block card) {
  bool ret_val = true;
  for (uint8_t i; i < 4; i++) {
    ret_val &= (card_id[i]==card.card_uid[i]);
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
    Serial.println("INV_POS");
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

void answer_rf() {
  //TODO: implement
}