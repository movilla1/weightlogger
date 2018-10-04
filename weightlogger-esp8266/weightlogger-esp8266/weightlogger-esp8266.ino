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
char selecteds[3] = {0,0,0};

void setup() {
  String ssid = WiFi.SSID();
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
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
  server.on("/ajax/server", handleAjaxServer);
  server.on("/ajax/tags", handleAjaxTags);
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
  digitalWrite(LED_BUILTIN, HIGH);
  bool wpsSuccess = WiFi.beginWPSConfig();
  if(wpsSuccess) {
    digitalWrite(LED_BUILTIN, LOW);
    return true;
  } else {
    digitalWrite(LED_BUILTIN, LOW);
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

/**
 * Do not use this function to load images, only for html or txt to render to the client.
 */
String getFromSpiffs(const String file) { 
  String content = "";
  char tmp;
  File dataFile = SPIFFS.open(file.c_str(), "r");
  while(dataFile.available()) {
    tmp = dataFile.read();
    content.concat(tmp);
  }
  dataFile.close();
  return content;
}

void change_selecteds(char opt) {
  memset(selecteds, 0, sizeof(selecteds));
  selecteds[opt] = 1;
}

void eeprom_store(String data, int address) {
  int x;
  int data_len = data.length();
  for (x=0; x < data_len; x++) {
    EEPROM.write(address + x, data.charAt(x));
  }
  EEPROM.write(address + data_len, ';'); //end the data with a semi-colon.
  EEPROM.commit(); 
}


/**
 * Use this to send images or big files to the client
 */
bool loadFromSpiffs(String path){
  String dataType = "text/plain";
  if(path.endsWith("/")) path += "index.htm"; 
  if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if(path.endsWith(".html")) dataType = "text/html";
  else if(path.endsWith(".css")) dataType = "text/css";
  else if(path.endsWith(".js")) dataType = "application/javascript";
  else if(path.endsWith(".jpg")) dataType = "image/jpeg";
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
  }
  dataFile.close();
  return true;
}
