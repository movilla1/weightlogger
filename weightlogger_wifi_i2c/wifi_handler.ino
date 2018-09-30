#include "constants.h"

/**
 * Module file to handle everything wifi related, with the AT command firmware on ESP8266
 **/

void initialize_wifi() {
  bool timeout=false;
  long tstart;
  Serial.print(F("ATE0\r\n"));
  Serial.flush();
  delay(5);
  tstart = millis();
  while (!Serial.available() && !timeout) {
    timeout = ((millis() - tstart) < WIFI_TIMEOUT);    
  }
  while(Serial.available()) {
    char tmp = Serial.read(); //just eat the bytes, ignore content for now.
  }
  if (timeout) {
    sysState = ERROR_WIFI;
    Serial.print(F("ATE1\r\n"));
  } else {
    Serial.print(F("AT+CWAUTOCONN=1\r\n"));
    Serial.flush();
    delay(1);
    empty_serial_buffer();
    Serial.print(F("AT+CIPMUX=1\r\n"));
    Serial.flush();
    delay(1);
    empty_serial_buffer();
    Serial.print(F("AT+CIPSERVER=1,80\r\n"));
    Serial.flush();
    delay(1);
    empty_serial_buffer();
  }
}

void send_wifi_wps_setup() {
  Serial.print(F("AT+CWMODE_DEF=1\r\n"));
  Serial.print(F("AT+WPS=1\r\n")); //start WPS
  char tmp = Serial.read();
  if (tmp=="O") {
    sysState = WPS_ON;
  } else {
    sysState = ERROR_WIFI;
  }
  empty_serial_buffer();
}

void send_wifi_wps_stop() {
  Serial.print(F("AT+WPS=0\r\n")); //stop WPS;
}

void get_ip_address() {
  byte count = 0;
  char t;
  pollData = 'I';
  empty_serial_buffer();
  Serial.print(F("AT+CIFSR\r\n"));
  Serial.flush();
  delay(1);
  while(Serial.available() && count < IP_START_POS_IN_RESPONSE + IP_ADDR_SIZE) {
    count++;
    if (count > IP_START_POS_IN_RESPONSE) { //
      t = Serial.read();
      wireBuffer[count - IP_START_POS_IN_RESPONSE] = t;
    }
  }
  empty_serial_buffer(); //drop the rest.
  count = count - IP_START_POS_IN_RESPONSE;
  wireBuffer[count] = 0x00; //end string at the last position.
}

/**
 * This function should receive the +IPD data pre-processed and extract the server IP
 **/
void setServerIp(char *ip_param) { 
  char tmp;
  byte pos = 0;
  bool exit = false;
  for(pos = 0; pos < IP_ADDR_SIZE; pos++) {
    tmp = ip_param[pos];
    EEPROM.write(IP_START_ADDR + pos, tmp);
    pos ++;
  }
}

void transmit_to_server() {
  char t;
  start_tcp_to_server();
  Serial.print(F("AT+CIPSENDEX=512\r\n"));
  Serial.flush();
  delay(1);
  send_base_http_request();
  Serial.print(F(" \r\n"));
  Serial.print(F("data="));
  while(Wire.available()) {
    t = Wire.read();
    Serial.write(t);
  }
  Serial.print(F(" \r\n"));
  Serial.write(0x00); //end transmission.
  stop_tcp_to_server();
  empty_serial_buffer(); //throw away the response (and hope for the best)
}

void send_base_http_request() {
  Serial.print(F("POST /pesaje/create_from_rfid HTTP/1.1\r\n"));
  Serial.flush();
  delay(1);
  Serial.print(F("User-Agent: Mozilla/5.0 (Weightlogger; es-AR; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5\r\n\
  Accept: text/html, application/xml, application/json;q=0.9;q=0.8;q=0.9\r\n\
  Accept-Language: en-us,en;q=0.5\r\n\
  Accept-Charset: ISO-8859-1;q=0.7\r\n\
  Content-Length: 64\r\n\
  Content-Type: application/x-www-form-urlencoded\r\n"));
}

void start_tcp_to_server() {
  Serial.print(F("AT+CIPSTART=\"TCP\",\""));
  Serial.print(serverIP);
  Serial.print(F("\",80"));
  Serial.print(F("\r\n"));
}

void stop_tcp_to_server() {
  Serial.print(F("AT+CIPCLOSE=5\r\n"));//close all connections
  Serial.flush();
  delay(1);
}

void processIPD() {
  byte pos = 0;
  int span = 0;
  char header[16];
  char cmd[7];
  char param[16];

  span = strcspn(ipdBuffer, ":");
  strncpy(header, ipdBuffer, span);
  header[span] = 0x00;
  link = header[5];
  if (strcmp(header,"+IPD") == 0) {
    digitalWrite(LED, HIGH);
    strncpy(cmd, ipdBuffer + span + 1, COMMAND_SIZE);
    cmd[6] = 0x00;
    strncpy(param, ipdBuffer + span + 1 + COMMAND_SIZE + SKIP_SIZE, PARAM_SIZE); 
    param[PARAM_SIZE]=0x00;
    if (strcasecmp(cmd,"SERVER")==0) {
      setServerIp(param);
      SendWifiClose(F("OK IPA"));
    } else if (strcasecmp(cmd,"ADDTAG")==0) {
      storeTag(param);
      SendWifiClose(F("OK TAG"));
    } else {
      SendWifiClose(F("FAILED"));
    }
    digitalWrite(LED, LOW);
  }
}

void storeTag(char *tagParam) {
  pollData = 'T';
  memcpy(wireBuffer, tagParam, PARAM_SIZE);
}

void SendWifiClose(const __FlashStringHelper *what) {
  Serial.print(F("AT+CIPSEND="));
  Serial.print(link);
  Serial.print(F(",6\r\n"));
  Serial.print(what);
  Serial.print(F("\r\n"));
}
