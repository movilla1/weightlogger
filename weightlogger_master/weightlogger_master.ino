#define DEBUG true
//#define WITH_WEIGHT true
#define WITH_WIFI true
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#include <MFRC522.h>  // Library for Mifare RC522 Devices
#include <LiquidCrystal_I2C.h>

#include "elcan_wifi.h"
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
  pinMode(BUZZER, OUTPUT);
  pinMode(RFID_SS, OUTPUT);
  pinMode(RFID_RST, OUTPUT);
  digitalWrite(BARRERA, LOW);
  digitalWrite(BUZZER, LOW);
#ifdef WITH_WEIGHT
  scale.begin(SCALE_I2C_ADDR); // according to wheight measurement device
#endif
  SPI.begin();           // MFRC522 Hardware uses SPI protocol
  Wire.begin();
  lcd.begin(16,2);
  initialize_rtc();
  initialize_rfid();
  sys_state = READY;
  whos_entering = 0;
  backlightStart = 0;
  lastPoll = 0;
  delay(5000);
#ifdef WITH_WIFI
  Serial.begin(115200, SERIAL_8N1);
  if (wifi.begin()) {
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
      } else {
        check_wifi();
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
      scale.get_weight(measuredWeight);
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
    case GET_TAG_DATA:
      get_tag_data();
      sys_state = READY;
      break;
    case ADJUST_TIME:
      adjust_rtc();
      sys_state = READY;
      break;
  }
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
  char tmp[20];
  char intTmp[4];
  long time = enteringTime.secondstime();
  memset(tmp, 0, sizeof(tmp));
  memset(intTmp, 0, sizeof(intTmp));
  dec_to_str(intTmp, whos_entering, sizeof(whos_entering));
  strcat(tmp, intTmp);
  dec_to_str(intTmp, time, sizeof(time));
  strncat(tmp, intTmp, sizeof(intTmp));
  dec_to_str(intTmp, measuredWeight, sizeof(measuredWeight));
  strncat(tmp, intTmp, sizeof(intTmp));
  wifi.sendEntry(tmp);
}

void send_intrussion_attemp_to_server(){
  char tmp[10];
  long time = enteringTime.secondstime();
  memset(tmp, 0, sizeof(tmp));
  memcpy(tmp, readCard, sizeof(readCard));
  memcpy(tmp+4, &time, sizeof(time) );
  const byte dataLength = sizeof(tmp) + sizeof(readCard) + sizeof(time);
  wifi.sendIntrussionAttemp(tmp);
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

void do_known_beeps() {
  for (byte b=0; b<2; b++) {
    digitalWrite(BUZZER, HIGH);
    delay(80);
    digitalWrite(BUZZER, LOW);
    delay(80);
  }
}

void get_tag_data() {
  char tagPacket[TAG_PACKET_SIZE];
  char pos;
  byte result[4];
  byte remov;

  memset(tagPacket, 0, sizeof(tagPacket));
  wifi.readCardData(tagPacket, sizeof(tagPacket));
  struct card_block card;
  pos = tag_string_to_bytes(tagPacket, result, &remov);
  memcpy(card.card_uid, result, sizeof(result));
  card.card_number = pos;
#ifdef DEBUG
  Serial.print("#");
  for(char c=0; c < 4; c++) {
    Serial.print(result[c], HEX);
  }
  Serial.print(F("\n"));
  Serial.println(pos, DEC);
  Serial.flush();
#endif
  if (remov == '0') {
    store_card(card, pos);
  } else {
    erase_card(pos);
  }
}

void erase_card(char pos) {
  if (pos > 200) //always inside the position limit.
    return;
  char ppos = (pos * sizeof(struct card_block)) + 1;
  for (char i=0; i<sizeof(struct card_block); i++) {
    EEPROM.write(ppos+i, 0xff);
    delay(5); // 5mS between byte writes, to allow the data to be written
  }
}

void dec_to_str (char* str, uint32_t val, size_t digits)
{
  size_t i=1;

  for(; i<=digits; i++)
  {
    str[digits-i] = (char)((val % 10) + '0');
    val/=10;
  }
  str[i-1] = '\0'; // assuming you want null terminated strings?
}

void check_wifi() {
  char polled;
  if (millis() - lastPoll > POLLING_INTERVAL) {
    polled = wifi.poll();
    lcd.print(polled);
    switch(polled) {
      case 'T':
        sys_state = GET_TAG_DATA;
        break;
      case 'R':
        sys_state = ADJUST_TIME;
        break;
    }
    lastPoll = millis();
  }
}

byte tag_string_to_bytes(char *tagstring, byte *tag_uid, byte *remove) {
  byte posBuf[4];
  byte tmp;
  memset(posBuf,0,sizeof(posBuf));
  hex_string_to_byte_array(tagstring, posBuf, TAG_UID_START, TAG_UID_END);
  memcpy(tag_uid, posBuf, sizeof(posBuf));
  hex_string_to_byte_array(tagstring, posBuf, TAG_POS_START, TAG_POS_END);
  tmp = posBuf[0]; //after converting the pos HEX to bin, it'll use 1 byte only.
  remove[0] = tagstring[TAG_REM_START];
  return tmp;
}

void hex_string_to_byte_array(char *src, byte *out_array, byte start, byte end) {
  char pos = 0;
  char tmp;
  for (byte c = start; c < end; c += 2) {
    tmp = src[c] > 0x39 ? ((src[c] - 'A') + 10) * 16 : (src[c] - '0') * 16;
    tmp += src[c+1] > 0x39 ? ((src[c+1] - 'A') + 10) : (src[c+1] - '0');
    out_array[pos] = tmp;
    pos ++;
  }
}
