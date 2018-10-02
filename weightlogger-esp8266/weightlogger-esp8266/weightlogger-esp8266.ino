#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "defines.h"

byte sysState;
ESP8266WebServer server(8010);  //  port 8010 = web, own
HTTPClient http;

void setup() {
  String ssid = WiFi.SSID();
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  
  if (ssid.length() > 1) { // if we have the ssid set, wps ran before, let's use it
    WiFi.begin(WiFi.SSID().c_str(),WiFi.psk().c_str()); // reading data from EPROM, 
    while (WiFi.status() == WL_DISCONNECTED) {          // last saved credentials
      delay(500);
    }
  }
  SPIFFS.begin();
    server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.on("/server", handleServerIP);
  server.on("/tags", handleTags);
  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);
  server.begin();  // our web server
  EEPROM.begin(512);
}

void loop() {
  char cmd;
  server.handleClient();
 
  if (Serial.available()) {
    cmd = Serial.read(); //commands are single chars
    switch (cmd) {
      case 'T':
        //get Tag
        break;
      case 'I':
        Serial.println(WiFi.localIP());
        break;
      case 'E':
        send_server_ip();
        break;
      case 'P':
        send_status();
        break;
      case 'S':
        //send data to server
        if (transmit_to_server()) {
          Serial.println("OK");
        } else {
          Serial.println("FAIL");
        }
        break;
      case 'W':
        //do the WPS procedure
        if (do_wps_setup()) {
          Serial.println("OK");
        } else {
          Serial.println("FAIL");
        }
        break;
    }
  }
}

void send_server_ip() {
  char ip[16];
  read_ip_from_eeprom(ip, sizeof(ip));
  Serial.println(ip);
}

void send_status() {
  switch (WiFi.status()) {
    case WL_CONNECTED:
      Serial.println("OK");
      break;
    case WL_DISCONNECTED:
      Serial.println("NN");
      break;
    default:
      Serial.println("UN");
      break;
  }
}

bool transmit_to_server() {
  char ip[16];
  char port[4];
  read_ip_from_eeprom(ip, sizeof(ip));
  read_port_from_eeprom(port,sizeof(port));
  String server = "http://";
  server += ip;
  server += port;
  server += "/pesaje/create_from_rfid"; 
  http.begin(server);
  String dat="data=";
  while(char tmp = Serial.read() != '\n') {
    dat += tmp;
  }
  int httpCode = http.POST(dat);
  return httpCode == 200;
}

bool do_wps_setup() {
  bool wpsSuccess = WiFi.beginWPSConfig();
  if(wpsSuccess) {
    return true;
  } else {
    return false;
  }
}

void read_ip_from_eeprom(char *ip, char len) {
  read_data_from_eeprom(ip, len, SERVER_IP_STORAGE_ADDR);
}

void read_port_from_eeprom(char *port, char len) {
  read_data_from_eeprom(port, len, SERVER_PORT_STORAGE_ADDR);
}

void read_data_from_eeprom(char *dat, char len, int start_position) {
 for(char x=0; x < len; x++) {
  dat[x] = EEPROM.read(start_position + x);
  if (dat[x]==0xFF) {
    dat[x]=' ';
  }
  if (dat[x]==';') {
    dat[x] = 0x00;
    break;
  }
 }
}
