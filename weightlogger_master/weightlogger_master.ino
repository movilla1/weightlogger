#define DEBUG true
//#define WITH_WEIGHT true
//#define WITH_WIFI true
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#include <MFRC522.h>  // Library for Mifare RC522 Devices
#include <LiquidCrystal_I2C.h>

#include "elcan_wifi_i2c.h"
#include "eepromblock.h"
#include "definitions.h"
#include "globals.h"
/**
 * System setup
 */
void setup() {
  pinMode(WIFI_RX, INPUT);
  pinMode(WIFI_TX, OUTPUT);
  pinMode(BARRERA, OUTPUT);
  pinMode(WPS_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  pinMode(RFID_SS, OUTPUT);
  pinMode(RFID_RST, OUTPUT);
  digitalWrite(BARRERA, LOW);
  digitalWrite(BUZZER, LOW);
#ifdef WITH_WEIGHT
  Serial.begin(4800, SERIAL_8N1); // according to wheight measurement device
#endif
#ifdef DEBUG
  Serial.begin(57600);
#endif
  SPI.begin();           // MFRC522 Hardware uses SPI protocol
  Wire.begin();
  lcd.begin(16,2);
  initialize_rtc();
  initialize_rfid();
#ifdef WITH_WIFI
  initialize_wifi();
#endif
  sys_state = READY;
  whos_entering = 0;
  backlightStart = 0;
#ifdef WITH_WIFI
  if (wifi.begin(WIFI_I2C_ADDR)) {
    lcd_show_ip();
  } else {
    sys_state = ERROR_WIFI;
  }
#endif
}

void loop() {
  bool tmp;
  check_lcd_light();
  switch(sys_state) {
    case ERROR_WIFI:
    case ERROR_RFID:
    case ERROR_RTC:
    case ERROR_INVALID:
      show_error(sys_state);
      break;
    case READY:
      lcd_show_ready();
      if (getID()) {
        sys_state = READ_RFID;
      }
#ifdef DEBUG
      if (Serial.available()){
        serialOptions();
      }
#endif
      if (wifi.available()) {
        sys_state = DATA_LINK;
      }
      break;
    case READ_RFID:
      check_card_and_act(); //checks the card and if its valid, it starts the sequence
      break;
    case READ_RTC:
      lcd_show_allowed();
      read_rtc_value();
      sys_state = READ_WEIGHT;
      break;
    case READ_WEIGHT:
      lcd_show_wait();
 #ifdef WITH_WEIGHT
      read_weight();
 #endif
      sys_state = WRITE_RECORD;
      break;
    case WRITE_RECORD:
#ifdef WITH_WIFI    
      send_to_server();
#endif
      timerStarted = rtc.now();
      sys_state = TIMED_WAIT;
      break;
    case TIMED_WAIT:
      if (check_elapsed_time()) {
        sys_state = OPEN_BARRIER;
      }
      break;
    case OPEN_BARRIER:
      lcd_show_go();
      open_barrier();
      lcd_light_on();
      sys_state = READY;
      break;
    case UNKNOWN_CARD:
      alertUnknown();
      sys_state = READY;
      break;
    case DATA_LINK:
      //TODO: Implement again
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
    do_known_beeps();
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
  int pos = (5 * position) + 1;
  if ( position < 200) {  //if we are not full capacity
    EEPROM_writeBlock(pos, card); //store the card
  } else {
    sys_state = ERROR_INVALID;
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

void send_to_server() {
  //TODO : Implement.
}

void send_intrussion_attemp_to_server(){
  //TODO : Implement
}

void alertUnknown() {
  lcd_show_message(F("Acceso negado,  Informando..."));
  send_intrussion_attemp_to_server();
  for (uint8_t i=0; i<3; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(150);
    digitalWrite(BUZZER, LOW);
    delay(150);
  }

}

void lcd_show_ready() {
  char dateString[15];
  lcd.setCursor(0,0);
  DateTime tstamp = rtc.now();
  sprintf(dateString, "%02d/%02d/%04d %02d:%02d",tstamp.day(),tstamp.month(),tstamp.year(),tstamp.hour(),tstamp.minute());
  lcd.print(dateString);
  lcd.setCursor(0,1);
  lcd.print(F("Esperando..."));
}

void lcd_show_ip() {
#ifdef WITH_WIFI  
  String ip = wifi.get_ip();
  lcd_show_message(ip);
  delay(2000); //2 seconds delay to read the ip
#else
  lcd_show_message("Initialized");
#endif
}

void lcd_show_allowed() {
  lcd_show_message(F("Acceso permitido"));
}

void lcd_show_wait() {
  lcd_show_message(F("Espere por favor..."));
}

void lcd_show_go() {
  lcd_show_message(F("Avance..."));
}

void lcd_show_message(String message) {
  byte len = 0;
  lcd.clear();
  lcd.backlight();
  backlightStart = millis();
  String msg = message;
  if (msg.length() > 16) {
    lcd.setCursor(0,0);
    lcd.print(msg.substring(0,16));
    lcd.setCursor(0,1);
    lcd.print(msg.substring(16, msg.length()));
  } else {
    lcd.print(msg);
  }
}

void check_lcd_light() {
  if (backlightStart > 0) {
    if (millis() - backlightStart > LIGHT_DURATION) {
      lcd.noBacklight();
      backlightStart = 0;
    }
  }
}

void lcd_light_on() {
  lcd.backlight();
  backlightStart = millis();
}

void do_known_beeps() {
  for (byte b=0; b<2; b++) {
    digitalWrite(BUZZER, HIGH);
    delay(80);
    digitalWrite(BUZZER, LOW);
    delay(80);
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
      debug_store_card(readed);
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
  }
}

void debug_store_card(byte *card_data) {
  bool finish = false;
  bool timeout = false;
  long tstart;
  tstart = millis();
  struct card_block card;
  while (!finish || timeout)
  {
    if (millis() - tstart > MAX_CARD_WAIT_TIME)
    {
      timeout = true;
    }
    if (getID()) {
      card.card_number = card_data[1];
      memcpy(card.card_uid, readCard, 4);
      store_card(card, card_data[0]);
      finish = true;
    }
  }
}
#endif
