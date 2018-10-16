#define DEBUG 1
#include <Wire.h>
#include <EEPROM.h>
#include "constants.h"

volatile byte sysState;
char wireBuffer[BUFFER_SIZE];
char serverIP[IP_ADDR_SIZE+1]; //server ip max: 255.255.255.255 (kept in ascii format) 15 characters + 0x00 string end
char link; //for the wifi cipmux link identifier
char pollData;
unsigned long tStart;
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
  memset(serverIP, 0, sizeof(serverIP));
  memset(wireBuffer, 0, sizeof(wireBuffer));
  pollData = 'E';
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
      delay(1500);
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
      if (wifi_connected == true || tElapsed > WPS_TIMEOUT) {
        digitalWrite(LED, LOW);
        sysState = READY;
      }
      empty_serial_buffer();
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
        sysState = SEND_IP_ADDRESS;
        clearBuffer = true;
        pollData = 'I';
        break;
      case 'S': //Get Server IP
        sysState = SEND_SERVER_IP;
        pollData = 'E';
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
      case 'Q':
        sysState = GET_TAG_DATA;
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
      break;
    case SEND_INIT_DATA:
      Wire.write("INIOK");
      pollData = 'A';
      break;
    case SEND_SERVER_IP:
      getServerIP();
      Wire.write(serverIP);
      break;
    case SEND_IP_ADDRESS:
      get_ip_address();
      Wire.write(wireBuffer);
      break;
    case GET_TAG_DATA:
      getTagData();
      Wire.write(wireBuffer);
      break;
    default:
      Wire.write(0);
  }
  sysState = READY;
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

void empty_serial_buffer() {
  char tmp;
  while (Serial.available()) {
    tmp = Serial.read(); // read so we empty the buffer
  }
}

void getTagData() {
  char tmp[20];
  char pos = 0;
  memset(tmp, 0x00, sizeof(tmp));
  Serial.print(F("T\r\n"));
  Serial.flush();
  delay(5);
  while(Serial.available()) {
    tmp[pos] = Serial.read();
    pos ++;
    pos %= 20; //circular buffer, should not reach the return point under normal operations.
  }
  tmp[pos] = 0x00;
  memcpy(wireBuffer, tmp, strlen(tmp));
}

void getServerIP() {
  pollData = 'S';
  char ip[IP_ADDR_SIZE+1];
  memset(ip, 0, sizeof(ip));
  Serial.println("E");
  Serial.flush();
  delay(5);
  Serial.readBytesUntil('\r', ip, sizeof(ip) - 1);
}
