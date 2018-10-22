#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "defines.h"

byte sysState;
ESP8266WebServer server(8010);  //  port 8010 = web, own
char selecteds[] = {0,0,0,0};
String message;
bool tagReady;
char tag[7];

void setup() {
  tagReady = false;
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
  server.on("/server", handleServer);
  server.on("/tags", handleTags);
  server.on("/ajax/server", handleAjaxServer);
  server.on("/ajax/msg", handleAjaxMessages);
  server.on("/receive/tag", handleSentTag);
  server.on("/password", handlePasswords);
  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);
  server.begin();  // our web server
  EEPROM.begin(512); //512 Bytes for EEPROM storage
}

void loop() {
  char cmd;
  server.handleClient();
 
  if (Serial.available()) {
    cmd = Serial.read(); //commands are single chars
    switch (cmd) {
      case 'T':
        send_tag();
        break;
      case 'I':
        Serial.println(WiFi.localIP());
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
      case 'Q':
        Serial.println("INIOK");
        break;
    }
  }
}

void send_status() {
  if (tagReady) {
    Serial.print("T");
    return;
  }
  switch (WiFi.status()) {
    case WL_CONNECTED:
      Serial.println("O");
      break;
    case WL_DISCONNECTED:
      Serial.println("N");
      break;
    default:
      Serial.println("U");
      break;
  }
}

bool transmit_to_server() {
  HTTPClient http;
  char url[MAX_URL_LEN+1];
  char tmp;
  String dat="data=";
  const char len = MAX_URL_LEN;
  read_data_from_eeprom(url, len, SERVER_URL_STORAGE_ADDR);
  tmp = 0;
  while(tmp != '\n') {
    if (Serial.available()) {
      tmp = Serial.read();
      if (tmp > 0 && tmp!='\n')
        dat += tmp;
    }
  }
  dat += "&auth=32\r\n";
  http.writeToStream(&Serial);
  http.begin(url);
  http.addHeader("Content-Type","application/x-www-form-urlencoded");
  int httpCode = http.POST(dat);
  String payload = http.getString();
  http.end();
  return httpCode == 200;
}

bool do_wps_setup() {
  digitalWrite(LED_BUILTIN, LOW);
  bool wpsSuccess = WiFi.beginWPSConfig();
  if(wpsSuccess) {
    digitalWrite(LED_BUILTIN, HIGH);
    return true;
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    return false;
  }
}

void read_data_from_eeprom(char *dat, char len, int start_position) {
  for(char x=0; x < len; x++) {
    dat[x] = EEPROM.read(start_position + x);
    if (dat[x]==0xFF || dat[x] == ';') {
      dat[x] = 0x00;
      if (dat[x]==';') {
        break;
      }
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
  if (!SPIFFS.exists(path)) {
    return false;
  }
  if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if(path.endsWith(".html")) dataType = "text/html";
  else if(path.endsWith(".css")) dataType = "text/css";
  else if(path.endsWith(".js")) dataType = "text/javascript";
  else if(path.endsWith(".jpg")) dataType = "image/jpeg";
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
  } 
  dataFile.close();
  return true;
}
/**
 * returns the stored pass for the users identified by the number
 * @param char identifier can be:
 * - 0 = admin
 * - 1 = remote
 */ 
String getPassword(char identifier) {
  char pass[PASSWORD_MAX_LENGTH+1];
  int position;
  memset(pass, 0, sizeof(pass));
  position = (identifier==ADMIN_USR) ? PASSWORD_STORAGE0 : PASSWORD_STORAGE1;
  read_data_from_eeprom(pass, PASSWORD_MAX_LENGTH, position);
  String read_pass = pass;
  if (pass[0]==0xFF || pass[0]==0x00) {
    read_pass = "elcanAdmin!"; //default password for any case when empty.
  }
  return read_pass;
}

void send_tag() {
  if (tagReady) {
    Serial.println(tag);
    tagReady = false;
    memset(tag, 0, sizeof(tag)); //reset to 0
  }
}
