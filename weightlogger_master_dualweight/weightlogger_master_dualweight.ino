//#define DEBUG true
#define WITH_WEIGHT true
#define WITH_WIFI true
#define DUAL_WEIGHT true
#include "includes/globals.h"

/**
 * ISR Routine
 */
void actOnButton() {
  byte x;
  for (x = 0; x< 255; x++); //mini delay for bounce & spureous elimination
  if (digitalRead(PUSH_BUTTON)==0) {
    buttonPressed = true;
  }
}

/**
 * System setup
 */
void setup() {
  char ipaddr[30];
  memset(ipaddr, 0, sizeof(ipaddr));
  pinMode(WIFI_RX, INPUT);
  pinMode(WIFI_TX, OUTPUT);
  pinMode(BARRERA, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RFID_SS, OUTPUT);
  pinMode(RFID_RST, OUTPUT);
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON), actOnButton, FALLING);
  digitalWrite(BARRERA, LOW);
  digitalWrite(BUZZER, LOW);
#ifdef WITH_WEIGHT
  scale.begin(SCALE_I2C_ADDR); // according to wheight measurement device
#endif
  Wire.begin();
  SPI.begin();           // MFRC522 Hardware uses SPI protocol
  rtc.initialize();
  rfid.begin();
  sys_state = READY;
  backlightStart = 0;
  lastPoll = 0;
  elcanLcd.init();
  elcanLcd.welcome_message();
#ifdef WITH_WIFI
  delay(9000);
  Serial.begin(115200, SERIAL_8N1);
  if (wifi.begin()) {
    wifi.get_ip(ipaddr);
    elcanLcd.show_ip(ipaddr);
  } else {
    sys_state = ERROR_WIFI;
  }
#else
  elcanLcd.show_message("Initialized...");
#endif
}

void loop() {
  char dateString[25];
  DateTime tstamp;
  bool tmp;
  byte ret;
  elcanLcd.check_light();
  tstamp = rtc.now();
  switch(sys_state) {
    case ERROR_WIFI:
    case ERROR_RFID:
    case ERROR_RTC:
    case ERROR_INVALID:
      elcanLcd.show_error(sys_state);
      break;
    case READY:
      sprintf(dateString, "%02d/%02d/%04d %02d:%02d", tstamp.day(), tstamp.month(),
      tstamp.year(), tstamp.hour(), tstamp.minute());
      elcanLcd.show_ready(dateString);
      if (rfid.getID()) {
        sys_state = READ_RFID;
      } else {
        check_wifi();
      }
      break;
    case READ_RFID:
      ret = is_known_card(rfid.readCard);
      if (ret > 0) {
        sys_state = READ_RTC;
        DO_KNOWN_BEEPS;
      } else {
        sys_state = UNKNOWN_CARD;
      }; //checks the card and if its valid, it starts the sequence
      break;
    case READ_RTC:
      enteringTime = tstamp;
      sys_state = READ_WEIGHT;
      break;
    case READ_WEIGHT:
#if DUAL_WEIGHT
      elcanLcd.show_message("Proximo eje...");
#else
      elcanLcd.show_message("Espere por favor");
#endif
#ifdef WITH_WEIGHT
      scale.get_weight(measuredWeight);
  #ifdef DUAL_WEIGHT
      buttonPressed = false;
      sys_state = READ_WEIGHT2;
  #endif
#endif
#if (!WITH_WEIGHT && !DUAL_WEIGHT)
      sys_state = WRITE_RECORD;
#endif
      break;
#ifdef DUAL_WEIGHT
    case READ_WEIGHT2:
      DO_OPEN_BARRIER
      while(!buttonPressed); //wait until the driver pushes the button
      delay(2000); //wait 2 more seconds when the driver pushed the button
      scale.get_weight(secondWeight);
      sys_state = WRITE_RECORD;
      break;
#endif
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
      elcanLcd.show_message("Avance...");
      DO_OPEN_BARRIER
      sys_state = READY;
      break;
    case UNKNOWN_CARD:
      elcanLcd.show_message("Acceso negado,  Informando...");
      DO_INTRUSSION_BEEPS;
      delay(1000);
      #ifdef WITH_WIFI
        send_intrussion_attemp_to_server();
      #endif
      sys_state = READY;
      break;
    case GET_TAG_DATA:
      get_tag_data();
      sys_state = READY;
      break;
    case ADJUST_TIME:
      rtc.adjust_from_serial();
      sys_state = READY;
      break;
  }
}

bool check_elapsed_time() {
  DateTime current = rtc.now();
  TimeSpan diff_time = (current - timerStarted);
  if (diff_time.totalseconds() >= WAITING_TIME) {
    return true;
  }
  return false;
}

void send_to_server() {
  char tmp[48];
  char timestr[21];
#ifdef WITH_WIFI
  memset(tmp, 0, sizeof(tmp));
  memset(timestr, 0, sizeof(timestr));
  sprintf(tmp, "%02x%02x%02x%02x*", rfid.readCard[0], rfid.readCard[1], rfid.readCard[2], rfid.readCard[3]);
  sprintf(timestr, "%04d-%02d-%02d %02d:%02d:%02d", enteringTime.year(), enteringTime.month(),
    enteringTime.day(), enteringTime.hour(), enteringTime.minute(), enteringTime.second());
  strcat(tmp, timestr);
  strcat(tmp, "*");
  strncat(tmp, (char *)measuredWeight, 6);
#ifdef DUAL_WEIGHT
  strcat(tmp, "*");
  strncat(tmp, (char *)secondWeight, 6);
#endif
  wifi.sendEntry(tmp);
#endif
}

void send_intrussion_attemp_to_server(){
  char tmp[48];
  char timestr[21];
#ifdef WITH_WIFI
  DateTime stamp = rtc.now();
  memset(tmp, 0, sizeof(tmp));
  sprintf(tmp, "%02x%02x%02x%02x", rfid.readCard[0], rfid.readCard[1], rfid.readCard[2], rfid.readCard[3]);
  strcat(tmp, "*");
  sprintf(timestr, "%04d-%02d-%02d %02d:%02d:%02d", stamp.year(), stamp.month(),
    stamp.day(), stamp.hour(), stamp.minute(), stamp.second());
  strcat(tmp, timestr);
  wifi.sendIntrussionAttemp(tmp);
#endif
}

void get_tag_data() {
  char tagPacket[TAG_PACKET_SIZE];
  byte pos;
  byte result[4];
  byte remov;

  memset(tagPacket, 0, sizeof(tagPacket));
#ifdef WITH_WIFI  
  wifi.readCardData(tagPacket, sizeof(tagPacket));
#endif
  struct card_block card;
  pos = tag_string_to_bytes(tagPacket, result, &remov);
  memcpy(card.card_uid, result, sizeof(result));
  card.card_number = pos;
  if (remov == '0') {
#ifdef DEBUG
    Serial.print("#Storing @ ");
    Serial.println(pos);
#endif
    delay(400);
    if (!store_card(card, pos)) {
      sys_state = ERROR_INVALID;
    }
  } else {
#ifdef DEBUG
    Serial.println("#Erasing @ ");
    Serial.println(pos);
#endif
    erase_card(pos);
  }
}

void check_wifi() {
#ifdef WITH_WIFI
  byte polled;
  if (millis() - lastPoll > POLLING_INTERVAL) {
    polled = wifi.poll();
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
#endif
}

char tag_string_to_bytes(char *tagstring, byte *tag_uid, byte *remove) {
  char posBuf[4];
  char tmp;
  memset(posBuf,0,sizeof(posBuf));
  hex_string_to_byte_array(tagstring, posBuf, TAG_UID_START, TAG_UID_END);
  memcpy(tag_uid, posBuf, sizeof(posBuf));
  hex_string_to_byte_array(tagstring, posBuf, TAG_POS_START, TAG_POS_END);
  tmp = posBuf[0]; //after converting the pos HEX to bin, it'll use 1 byte only.
  remove[0] = tagstring[TAG_REM_START];
  return tmp;
}
