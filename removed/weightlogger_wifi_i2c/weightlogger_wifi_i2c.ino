#define DEBUG 1
#include <Wire.h>
#include <EEPROM.h>
#include "constants.h"

unsigned long tStart;
char wireBuffer[BUFFER_SIZE];
char tagBuffer[TAG_BUFFER_SIZE];
char pollData;
volatile byte sysState;
bool newData;
bool wifi_connected;


void setup() {
  Wire.begin(I2C_ADDR);
  pinMode(LED, OUTPUT);
  pinMode(SETUP_BTN, INPUT_PULLUP);
  digitalWrite(LED, HIGH);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestService);
  attachInterrupt(1, startSetup, FALLING);
  Serial.begin(115200); //start the serial interface
  delay(5000); //wait for the wifi interface to finish the initial settings/bootload.
  sysState = READY;
  initialize_wifi();
  memset(wireBuffer, 0, sizeof(wireBuffer));
  memset(tagBuffer, 0, sizeof(tagBuffer));
  pollData = 'E';
  newData = false;
  digitalWrite(LED, LOW);
}

void loop() {
  unsigned long tElapsed, tCurrent;
  byte pos = 0;
  byte tmp[5];
  char t;
  tCurrent = millis();
  switch(sysState) {
    case WPS_SETUP_STARTED:
      delay(2000);
      if (digitalRead(SETUP_BTN)==0) {
        sysState = WPS_SETUP;
      } else {
        sysState = READY;
      }
      break;
    case WPS_SETUP:
      send_wifi_wps_setup();
      tStart = millis();
      digitalWrite(LED, HIGH);
      wifi_connected = false;
      sysState = WPS_ON;
      break;
    case WPS_ON:
      tElapsed = tCurrent - tStart;
      if (newData==true) {
        if (strcmp(wireBuffer, "OK")==0) {
          wifi_connected = true;
          newData = false;
        }
      }
      if (wifi_connected == true || tElapsed > WPS_TIMEOUT) {
        digitalWrite(LED, LOW);
        sysState = READY;
      }
      break;
    case ERROR_WIFI:
      showError();
      break;
  }
}

void receiveEvent(int count) {
  bool clearBuffer = false;
  if (Wire.available()) {
    char cmd = Wire.read(); // read the command code;
    switch (cmd) {
      case 'G': //get IP address
        get_ip_address();
        sysState = SEND_IP_ADDRESS;
        clearBuffer = true;
        pollData = 'I';
        break;
      case 'T': //transmit data to Server
        transmit_to_server();
        break;
      case 'I': //initialization
        sysState = SEND_INIT_DATA;
        clearBuffer = true;
        break;
      case 'P': //poll status
        sysState = SEND_POLL_DATA;
        clearBuffer = true;
        break;
      case 'Q': //get tag
        sysState = GET_TAG_DATA;
        getTagData();
        pollData = 'T';
        clearBuffer = true;
        break;
    }
  }
  if (clearBuffer) {
    for (char tmp = 1; tmp < count; tmp++) { //clear the sent data, we only use 1 char as command.
      char t = Wire.read();
    }
  }
}

void requestService() {
  switch(sysState) {
    case SEND_POLL_DATA:
      Wire.write(pollData);
      sysState = READY;
      break;
    case SEND_INIT_DATA:
      Wire.write("INIOK");
      pollData = 'A';
      sysState = READY;
      break;
    case GET_TAG_DATA:
      Wire.write(tagBuffer);
      newData = false;
      pollData = 'E';
      sysState = READY;
      break;
    case SEND_IP_ADDRESS:
      newData = false;
      Wire.write(wireBuffer);
      pollData = 'E';
      sysState = READY;
      break;
    default:
      Wire.write(0);
      break;
  }
}

void startSetup() {
  sysState = WPS_SETUP_STARTED;
}

void showError() {
  digitalWrite(LED, HIGH);
  delay(150);
  digitalWrite(LED, LOW);
  delay(150);
}

void serialEvent() {
  char t;
  char size;
  char *output;
  switch (sysState) {
    case GET_TAG_DATA:
      size = MAX_TAG_SIZE;
      output = tagBuffer;
      break;
    case SEND_IP_ADDRESS:
      size = IP_ADDR_SIZE;
      output = wireBuffer;
      break;
    default:
      size = 30;
      break;
  }
  t = Serial.readBytesUntil('\n', output, size);
  output[t] = 0x00;
  newData = true;
}
