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
  content = replace_tag(content, "[[MSG]]", msg);
  server.send(200, "text/html", content);
}

//root page can be accessed only if authentification is ok
void handleRoot() {
  String header;
  if (!checkAuth()) {
    return;
  }
  change_selecteds(DASHBOARD);
  String content = render("/index.html","Welcome", selecteds, true);
  server.send(200, "text/html", content);
}

//no need authentification
void handleNotFound() {
  if(loadFromSpiffs(server.uri())) return;
  String message = "<h1>Not Found</h1>\n";
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
  message += "</ul>";
  String content = getFromSpiffs("/404.html");
  content = replace_tag(content, "[[CONTENT]]", message);
  server.send(404, "text/html", content);
}

void handleServerIP() { //handle server ip actions.
  if (!checkAuth()) {
    return;
  }
  String content = "";
  if (server.hasArg("IPA")) {
    String ip = server.arg("IPA");
    eeprom_store(ip, SERVER_IP_STORAGE_ADDR);
    String port = server.arg("port");
    eeprom_store(port, SERVER_PORT_STORAGE_ADDR);
    content += "Server IP Address set to:";
    content += ip;
    content += ":";
    content += port;
    content += "<br/><a href=\"/\">Back to home</a><br/>";
  } else {
    content +=  "";
  }
  change_selecteds(SETUP_IP_ADDRESS);
  String page = render("/server_ip.html", "Server IP", selecteds, true);
  server.send(200, "text/html", page);
}

void handleTags() { //handle tags operations
  if (!checkAuth()) {
    return;
  }
  change_selecteds(SETUP_TAGS);
  const String content = getFromSpiffs("/tags.html");
  String page = render (content, "Tag Manager", selecteds, false);
  server.send(200, "text/html", page);
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

void handleAjaxServer() {
  if (!checkAuth()) {
    return;
  }
  char ip[16];
  char port[4];
  String content;
  read_ip_from_eeprom(ip, sizeof(ip));
  read_port_from_eeprom(port,sizeof(port));
  content = "{\"ip\": \"";
  content += ip;
  content += "\", \"port\":\"";
  content += port;
  content += "\"}";
  server.send(200, "application/json", content);
}

void handleAjaxTags() {
  //TODO: Implement
  if (!checkAuth()) {
    return;
  }
  String content="{\"status\":\"OK\"}";
  server.send(200, "application/json", content);
}
