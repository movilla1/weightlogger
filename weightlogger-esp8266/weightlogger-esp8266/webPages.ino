#include "defines.h"

//login page, also called for disconnect
void handleLogin() {
  String msg="";
  if (server.hasHeader("Cookie")) {
    String cookie = server.header("Cookie");
  }
  if (server.hasArg("DISCONNECT")) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ELCANWLSV10=0");
    server.send(301);
    return;
  }
  if (server.hasArg("username") && server.hasArg("upwd")) {
    if (server.arg("username") == "admin" &&  server.arg("upwd") == "elcanAdmin!") {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ELCANWLSV10=SU1");
      server.send(301);
      return;
    }
    msg = "<div class=\"alert alert-danger\" role=\"alert\">Wrong username/password! try again.</div>\n";
  }
  String content = getFromSpiffs("/login.html");
  
  const String ending = "</div></body></html>";
  server.send(200, "text/html", content + msg + ending);
}

//root page can be accessed only if authentification is ok
void handleRoot() {
  String header;
  if (!checkAuth()) {
    return;
  }
  server.sendHeader("Location", "/index.html",true);   //Redirect to our html web page
  server.send(302, "text/plain", "");
}

//no need authentification
void handleNotFound() {
  if(loadFromSpiffs(server.uri())) return;
  String message = "<html><head><title>404 Not found</title></head><body><h1>Not Found</h1>\n\n";
  message += "<p>URI: ";
  message += server.uri();
  message += "</p>\n<p>Method: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "</p>\n<p>Arguments: ";
  message += server.args();
  message += "</p><ul>\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += "<li>" + server.argName(i) + ": " + server.arg(i) + "</li>\n";
  }
  message += "</ul></body></html>";
  server.send(404, "text/html", message);
}

void handleServerIP() { //handle server ip actions.
  if (!checkAuth()) {
    return;
  }
  char ip[16];
  char port[4];
  read_ip_from_eeprom(ip, sizeof(ip));
  read_port_from_eeprom(port,sizeof(port));
  String content = getFromSpiffs("/server_ip.html");
  if (server.hasArg("IPA")) {
    String ip = server.arg("IPA");
    int x;
    int ip_len = ip.length();
    for (x=0; x < ip_len; x++) {
      EEPROM.write(SERVER_IP_STORAGE_ADDR + x, ip.charAt(x));
    }
    EEPROM.write(SERVER_IP_STORAGE_ADDR + ip_len, ';'); //end the ip address with a semi-colon.
    EEPROM.commit(); 
    content += "Server IP Address set to:";
    content += ip;
    content += "<br/><a href=\"/\">Back to home</a><br/></div></div></body></html>";
  } else {
    content +=  "<h3>Setup Server IP Address</h3><form action=\"/server\" method=\"POST\"><div class=\"col-md-offset-5 col-md-3\"><div class=\"form-login\">\
    <input type=\"text\" id=\"ipa\" class=\"form-control input-sm chat-input\" name=\"IPA\" placeholder=\"Server IP Address\" value=\"";
    content += ip;
    content += "\"/> <input type=\"text\" id=\"port\" name=\"port\" value=\"";
    content += port;
    content += " placeholder=\"Server Port\"/> <br/><br/><div class=\"wrapper\"> <span class=\"group-btn\"><button class=\"btn btn-primary btn-md\"><span>Save</span>&nbsp;<i class=\"fas fa-save\"></i></button>\
    </span></div></div></div></form></div></div></body></html>";
  }
  server.send(200, "text/html", content);
}

void handleTags() { //handle tags operations
  if (!checkAuth()) {
    return;
  }
  const String content = getFromSpiffs("/tags.html");
  server.send(200, "text/html", content);
}

bool checkAuth() {
  if (!is_authentified()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return false;
  }
  return true;
}

//Check if header is present and correct
bool is_authentified() {
  if (server.hasHeader("Cookie")) {
    String cookie = server.header("Cookie");
    if (cookie.indexOf("ELCANWLSV10=SU1") != -1) {
      return true;
    }
  }
  return false;
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
