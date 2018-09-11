#include <Wire.h>
#include <EEPROM.h>

#define IP_START_POS_IN_RESPONSE 7
#define IP_START_ADDR 0x00
#define WIFI_TIMEOUT 200 //milliseconds timeout for wifi
#define WPS_TIMEOUT 4000 // 4 seconds after turning on wps we turn it off
#define BUFFER_SIZE 32

#define SETUP_BTN 3
#define LED 8
#define I2C_ADDR 0x18

#define READY 0
#define WPS_SETUP 1
#define I2C_GET 2
#define ERROR_WIFI 4
#define WPS_ON 8

byte sysState;
byte dataToSend, dataToWifi;
char dataBuffer[BUFFER_SIZE];
char serverIP[16]; //server ip max: 255.255.255.255 (kept in ascii format) 15 characters + 0x00 string end
void setup() {
  Wire.begin(I2C_ADDR);
  pinMode(LED, OUTPUT);
  pinMode(SETUP_BTN, INPUT_PULLUP);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestService);
  attachInterrupt(1, startSetup, FALLING);
  Serial.begin(115200); //start the serial interface
  initialize_wifi();
  sysState = READY;
  dataToSend = dataToWifi = 0;
  memset(serverIP, 0, sizeof(serverIP));
  eeprom_read_server_ip();
}

void loop() {
  long tStart, tElapsed;
  switch(sysState) {
    case WPS_SETUP:
      send_wifi_wps_setup();
      tStart = millis();
      break;
    case WPS_ON:
      tElapsed = millis() - tStart;
      if (tElapsed > WPS_TIMEOUT) {
        send_wifi_wps_stop();
      }
    case ERROR_WIFI:
      showError();
      break;
  }
}

void receiveEvent(int count) {
  if (Wire.available()) {
    char cmd = Wire.read(); // read the command code;
    switch (cmd) {
      case 'G': //get IP address
        get_ip_address();
        break;
      case 'S': //Set Server IP
        set_server_ip();
        break;
      case 'T': //transmit data to Server
        transmit_to_server();
        break;
      case 'I':
        dataToSend = 5;
        memcpy(dataBuffer,F("INIOK"), 5);
        break;
      case 'A':
        //This gets the known tags list.
        break;
    }
  }
}

void requestService() {
  if (dataToSend > 0) {
    Wire.write(dataBuffer);
    dataToSend = 0 ;
  }
}

void startSetup() {
  sysState = WPS_SETUP;
}

void initialize_wifi() {
  bool timeout;
  long tstart = millis();
  while(Serial.available() && !timeout) {
    char tmp = Serial.read(); //just eat the bytes, ignore content for now.
    timeout = ((millis() - tstart) < WIFI_TIMEOUT);
  }
  if (timeout) {
    sysState = ERROR_WIFI;
  } else {
    Serial.println(F("AT+CWAUTOCONN=1"));
    empty_serial_buffer();
  }
}

void showError() {
  digitalWrite(LED, HIGH);
  delay(150);
  digitalWrite(LED, LOW);
  delay(150);
}

void send_wifi_wps_setup() {
  Serial.println(F("AT+CWMODE_DEF=1"));
  Serial.println(F("AT+WPS=1")); //start WPS
  char tmp = Serial.read();
  if (tmp=="O") {
    sysState = WPS_ON;
  } else {
    sysState = ERROR_WIFI;
  }
  empty_serial_buffer();
}

void send_wifi_wps_stop() {
  Serial.println(F("AT+WPS=0")); //stop WPS;
}

void empty_serial_buffer() {
  char tmp;
  while (Serial.available()) {
    tmp = Serial.read(); // read so we empty the buffer
  }
}

void get_ip_address() {
  byte count = 0;
  char t;
  Serial.println(F("AT+CIFSR"));
  while(Serial.available()) {
    count++;
    if (count > IP_START_POS_IN_RESPONSE) { //
      t = Serial.read();
      dataBuffer[count - IP_START_POS_IN_RESPONSE] = t;
    }
  }
  dataToSend = count - IP_START_POS_IN_RESPONSE;
  dataBuffer[dataToSend] = 0x00; //end string at the last position.
}

void set_server_ip() {
  char tmp;
  byte pos = 0;
  bool exit = false;
  while(Wire.available() && !exit) {
    tmp = Wire.read();
    if (tmp=='\r') {
      exit = true;
    } else {
      EEPROM.write(IP_START_ADDR + pos, tmp);
      serverIP[pos] = tmp;
      pos ++;
      pos %= 15; // IP Address must be 15 characters or less
    }
  }
}

void transmit_to_server() {
  char t;
  //char command[]=F("AT+CIPSENDEX=512"); //512 bytes per packet, max.
  start_tcp_to_server();
  Serial.println(F("AT+CIPSENDEX=512"));
  send_base_http_request();
  Serial.println(F(" "));
  Serial.print(F("data="));
  while(Wire.available()) {
    t = Wire.read();
    Serial.write(t);
  }
  Serial.println(F(" "));
  Serial.write(0x00); //end transmission.
  stop_tcp_to_server();
  empty_serial_buffer(); //throw away the response (and hope for the best)
}

void send_base_http_request() {
  Serial.println(F("POST /pesaje/create_from_rfid HTTP/1.1"));
  Serial.println(F("User-Agent: Mozilla/5.0 (Weightlogger; es-AR; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5\r\n\
  Accept: text/html, application/xml, application/json;q=0.9;q=0.8;q=0.9\r\n\
  Accept-Language: en-us,en;q=0.5\r\n\
  Accept-Charset: ISO-8859-1;q=0.7\r\n\
  Content-Length: 64\r\n\
  Content-Type: application/x-www-form-urlencoded\r\n"));
}

void start_tcp_to_server() {
  char st_command_tail [] =  "\",80";
  String st_command = F("AT+CIPSTART=\"TCP\",\"");
  st_command += serverIP;
  st_command += st_command_tail;
  Serial.println(st_command);
}

void stop_tcp_to_server() {
  Serial.println(F("AT+CIPCLOSE=5"));//close all connections
}

void eeprom_read_server_ip() {
  char tmp;
  for (byte i=0; i < sizeof(serverIP)-1; i++) {
    tmp = EEPROM.read(IP_START_ADDR+i);
    if (tmp > '0' && tmp < '9') {
      serverIP[i] = tmp;
    }
  }
}