#define DEBUG true
//#define WITH_WEIGHT true
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#include <SD.h>
#include <MFRC522.h>  // Library for Mifare RC522 Devices
#include <RF24.h>

#include "eepromblock.h"
#include "definitions.h"
#include "rf_protocol.h"

struct card_block {
  byte card_uid[4];
  byte card_number;
};

/**
 * Global Variables
 */
File myFile;
RTC_DS3231 rtc;
MFRC522 mfrc522(RFID_SS, RFID_RST); //Creamos el objeto para el RC522
byte sys_state;
byte readCard[4];   // Stores scanned ID read from RFID Module
byte whos_entering; //stores in ram the card position that's readed
uint16_t measuredWeight; // Stores weight in ram
DateTime enteringTime;  //last time readed on the RTC
DateTime timerStarted;
ElcanProto protocolManager;

/* Radio related config */
byte addresses[][6] = {"ELCN1","ELCN2","ELCNM"};
RF24 radio(RADIO_CE, RADIO_SS); //RF24 Radio on pins 7 & 8
byte lastCommFrom;
/**
 * System setup
 */
void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BARRERA, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(SDCARD_SS, OUTPUT);
  pinMode(RADIO_CE, OUTPUT);
  pinMode(RADIO_SS, OUTPUT);
  pinMode(RFID_SS, OUTPUT);
  pinMode(RFID_RST, OUTPUT);
  digitalWrite(BARRERA, LOW);
  digitalWrite(LED, LOW);
  digitalWrite(BUZZER, LOW);
#ifdef DEBUG
  Serial.begin(57600, SERIAL_8N1); // according to wheight measurement device
#else
  Serial.begin(4800, SERIAL_8N1);
#endif
  SPI.begin();           // MFRC522 Hardware uses SPI protocol
  Wire.begin();
  initialize_rtc();
  selectSPI(SPI_RFID);
  initialize_rfid();
#ifdef DEBUG
  Serial.println("Radio init...");
#endif
  selectSPI(SPI_RADIO);
  initialize_radio();
  selectSPI(SPI_RFID);
#ifdef DEBUG
  Serial.println("Initialized");
#endif
  sys_state = READY;
  lastCommFrom = 0;
  whos_entering = 0;
}

void loop() {
  bool tmp;
  switch(sys_state) {
    case ERROR_SD:
    case ERROR_RFID:
    case ERROR_RTC:
      show_error(sys_state);
      break;
    case READY:
      selectSPI(SPI_RFID);
      if (getID()) {
        sys_state = READ_RFID;
      }
      selectSPI(SPI_RADIO);
      if (radio.available(&lastCommFrom)){
        sys_state = DATA_LINK;
      }
#ifdef DEBUG
      if (Serial.available()){
        serialOptions();
      }
#endif
      break;
    case READ_RFID:
      check_card_and_act(); //checks the card and if its valid, it starts the sequence
      break;
    case READ_RTC:
      read_rtc_value();
      sys_state = READ_WEIGHT;
      break;
    case READ_WEIGHT:
 #ifdef WITH_WEIGHT
      read_weight();
 #endif
      sys_state = WRITE_RECORD;
      break;
    case WRITE_RECORD:
      /*selectSPI(SPI_SDCARD);
      initialize_sd_card();
      write_values_to_file('O');
      SD.end();
      selectSPI(SPI_RFID);
      initialize_rfid();*/
      send_data_by_rf();
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
#ifdef DEBUG
      Serial.println("Ready");
#endif
      break;
    case UNKNOWN_CARD:
      alertUnknown();
      sys_state = READY;
      break;
    case DATA_LINK:
      rf_protocol_manager();
      break;
  }
}

uint8_t getID() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return 0;
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
  for ( uint8_t i = 0; i < 4; i++) {  //
    readCard[i] = mfrc522.uid.uidByte[i];
#ifdef DEBUG
    Serial.print(readCard[i], HEX);
#endif
  }
#ifdef DEBUG
  Serial.println(" ");
#endif
  mfrc522.PICC_HaltA(); // Stop reading
  return 1;
}
/**
 * Readed card must be checked agains the known ones
 */
bool check_card_and_act() {
  byte ret = is_known_card(readCard);
  if (ret > 0) {
    sys_state = READ_RTC;
    whos_entering = ret;
  } else {
    sys_state = UNKNOWN_CARD;
  }
  return ret;
}

/**
 * Read the time and store it in memory
 */
bool read_rtc_value() {
  uint32_t tmp = 0;
  enteringTime = rtc.now();
  return true;
}

/**
 * Read the weight and store it in memory
 */
void read_weight() {
  char weight_data[7];
  String tmp;
  bool finish = false;
  byte temp;
  long start;
  uint16_t pos;
  start = millis();
  while(!Serial.available() && !finish) {
    if (millis()-start > MAX_WEIGHT_WAIT_TIME) {
      finish = true;
    }
  }
  if (Serial.available() && !finish) {
    pos = 0;
    while( !finish) {  // wait for the weight to steady up,then read the next 7 bytes that will hold the actual weight
      temp = Serial.read();
      finish = ( temp == 0x1c || temp== 0x1b || pos > MAX_BYTES_WRONG);
      pos++;
    }
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

void open_barrier() {
  digitalWrite(BARRERA, 1);
  delay(8000); //wait until the barrier acknowledges the open command
  digitalWrite(BARRERA, 0); //release Barrier switch
}

bool compare_card(byte card_id[4], struct card_block card) {
  for (uint8_t i; i < 4; i++) {
    if (card_id[i] != card.card_uid[i]) {
      return false;
    }
  }
  return true;
}

byte is_known_card(byte card_id[]) {
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
      ret_val = 0;
    }
    pos += sizeof(card);
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
    answer_to_sender(false);
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

void alertUnknown() {
  for (uint8_t i=0; i<3; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(150);
    digitalWrite(BUZZER, LOW);
    delay(150);
  }
}

void selectSPI(const uint8_t opt) {
  switch(opt) {
/*    case SPI_SDCARD:
      digitalWrite(SDCARD_SS, LOW);
      digitalWrite(RADIO_SS, HIGH);
      digitalWrite(RFID_SS, HIGH);
      break;*/
    case SPI_RADIO:
      digitalWrite(SDCARD_SS, HIGH);
      digitalWrite(RADIO_SS, LOW);
      digitalWrite(RFID_SS, HIGH);
      break;
    case SPI_RFID:
      digitalWrite(SDCARD_SS, HIGH);
      digitalWrite(RADIO_SS, HIGH);
      digitalWrite(RFID_SS, LOW);
      break;
  }
}

#ifdef DEBUG
void serialOptions() {
  byte readed[2];
  byte cmd;
  byte buff[16];
  byte count;

  cmd = Serial.read();
  Serial.println("ACK");
  switch(cmd) {
    case 'S':
      Serial.readBytes(readed,2);
      Serial.println("Scan");
      readed[0] *= sizeof(card_block);
      readed[0] ++;
      rf_store_card(readed);
      Serial.println("Ready");
      break;
    case 'D':
      for (uint16_t i = 0; i < 0x400; i++)
      {
        buff[0] = EEPROM.read(i);
        Serial.print(buff[0],HEX);
        Serial.print(" ");
        if (i % 20 == 0) {
          Serial.println(" |");
        }
      }
      Serial.println("Dump finished");
      break;
    /*case 'C':
      selectSPI(SPI_SDCARD);
      initialize_sd_card();
      File myFile;
      myFile = SD.open("datalog.csv", O_READ);
      if (myFile) {
        while (myFile.available()) {
          Serial.write(myFile.read());
        }
        myFile.close();
      }
      SD.end();
      selectSPI(SPI_RFID);
      initialize_rfid();
      break;*/
  }
}
#endif

/*void write_values_to_file(char action) {
  char timestring[20];
  sprintf(timestring, "%04d/%02d/%02d %02d:%02d:%02d", enteringTime.year(), enteringTime.month(), 
    enteringTime.day(), enteringTime.hour(), enteringTime.minute(), enteringTime.second());
  myFile = SD.open("datalog.csv", O_WRITE | O_CREAT | O_APPEND);
  myFile.print(timestring);
  myFile.print(';');
  myFile.print(whos_entering, DEC);
#ifdef WITH_WEIGHT
  myFile.print(';');
  myFile.print(measuredWeight, DEC);
#endif
  myFile.print(';');
  myFile.println(action);
  myFile.close();
}*/
