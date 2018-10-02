#include <Wire.h>
#include <EEPROM.h>
#include "constants.h"
/**
 * ESP Data gathered in debug:
 * 
 * Server: answers multiple lines, the most significant part is
 *  +IPD,0,442:GET /SERVER?IP=192.168.025.102
 *  +IPD,0,430:GET /ADDTAG?TG=00:00:00:00,250
 * when we open a browser with /SERVER?IP=192.168.25.102, where
 *  0 is the link identifier, 442 is the length received.
 * 
 * CIFSR Answers with multiple lines, sample:
 * +CIFSR:STAIP,"192.168.25.10"
 * +CIFSR:STAMAC,"84:f3:eb:4c:cf:73"
 *
 * OK
 * We must process the STAIP part, ignore the rest.
 **/


byte sysState;
char wireBuffer[BUFFER_SIZE];
char serverIP[IP_ADDR_SIZE+1]; //server ip max: 255.255.255.255 (kept in ascii format) 15 characters + 0x00 string end
char ipdBuffer[IPD_BUFFER_SIZE];
char link; //for the wifi cipmux link identifier
char pollData;

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
  memset(serverIP, 0, sizeof(serverIP));
  memset(ipdBuffer, 0, sizeof(ipdBuffer));
  pollData = 'E';
  eepromReadServerIp();
  digitalWrite(LED, LOW);
}

void loop() {
  long tStart, tElapsed;
  byte pos = 0;
  char t;
  switch(sysState) {
    case WPS_SETUP:
      send_wifi_wps_setup();
      tStart = millis();
      digitalWrite(LED, HIGH);
      break;
    case WPS_ON:
      tElapsed = millis() - tStart;
      if (tElapsed > WPS_TIMEOUT) {
        send_wifi_wps_stop();
        digitalWrite(LED, LOW);
      }
    case ERROR_WIFI:
      showError();
      break;
  }
  if (Serial.available() && sysState==READY) { //WIFI is sending something for us:
    while (Serial.available()) {
      t = Serial.read();
      ipdBuffer[pos] = t;
      pos++;
      pos %= IPD_BUFFER_SIZE; //circular buffering
      if (t=='\n') {
        processIPD();
      }
    }
  } else {
    empty_serial_buffer();
  }
}

void receiveEvent(int count) {
  bool clearBuffer = false;
  if (Wire.available()) {
    char cmd = Wire.read(); // read the command code;
    switch (cmd) {
      case 'G': //get IP address
        sysState = SEND_IP_ADDRESS;
        clearBuffer = true;
        break;
      case 'S': //Get Server IP
        sysState = SEND_SERVER_IP;
        clearBuffer = true;
        break;
      case 'T': //transmit data to Server
        transmit_to_server();
        break;
      case 'I':
        sysState = SEND_INIT_DATA;
        clearBuffer = true;
        break;
      case 'P':
        sysState = SEND_POLL_DATA;
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
      break;
    case SEND_INIT_DATA:
      Wire.write("INIOK");
      pollData = 'E';
      break;
    case SEND_SERVER_IP:
      getServerIP();
      Wire.write(serverIP);
      pollData = 'E';
      break;
    case SEND_IP_ADDRESS:
      get_ip_address();
      Wire.write(wireBuffer);
      pollData = 'E';
      break;
    default:
      Wire.write(0);
  }
  sysState = READY;
}

void startSetup() {
  delay(400);
  if (digitalRead(SETUP_BTN)==0) { //if the button stays low for 400mS
    sysState = WPS_SETUP;
  } else {
    sysState = READY;
  }
}

void showError() {
  digitalWrite(LED, HIGH);
  delay(150);
  digitalWrite(LED, LOW);
  delay(150);
}

void empty_serial_buffer() {
  char tmp;
  while (Serial.available()) {
    tmp = Serial.read(); // read so we empty the buffer
  }
}

void eepromReadServerIp() {
  char tmp;
  for (byte i=0; i < sizeof(serverIP)-1; i++) {
    tmp = EEPROM.read(IP_START_ADDR+i);
    if (tmp > '0' && tmp < '9') {
      serverIP[i] = tmp;
    }
  }
}

void getServerIP() {
  pollData = 'S';
  eepromReadServerIp();
}
