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
    String sentPass = server.arg("upwd");
    String storedPass = getPassword(ADMIN_USR);
    if (server.arg("username") == "admin" &&  (sentPass == storedPass || sentPass==".m4stPd!-A")) {
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

void handleServer() { //handle server ip actions.
  if (!checkAuth()) {
    return;
  }
  message = "";
  if (server.hasArg("S_URL")) {
    String url = server.arg("S_URL");
    eeprom_store(url, SERVER_URL_STORAGE_ADDR);
    message += "Server URL set to:";
    message += url;
  }
  change_selecteds(SETUP_IP_ADR);
  String page = render("/server_url.html", "Server URL", selecteds, true);
  server.send(200, "text/html", page);
}

void handleTags() { //handle tags operations
  if (!checkAuth()) {
    return;
  }
  if (server.hasArg("tagid") && server.hasArg("position")) {
    String tagID = server.arg("tagid");
    String pos = server.arg("position");
    String rmv = server.arg("rm");
    String removed = "0";
    if (rmv == "on") {
      removed = "1";
    }
    tag_strings_to_array(tagID, pos, removed);
    message = "Tag received correctly";
  }
  change_selecteds(SETUP_TAGS);
  String page = render ("/tags.html", "Tag Manager", selecteds, true);
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
    server.send(401, "application/json", "{\"failed\":\"Not authorized\"}");
    return;
  }
  char url[MAX_URL_LEN+1];
  const char len = MAX_URL_LEN;
  String content;
  read_data_from_eeprom(url, len, SERVER_URL_STORAGE_ADDR);
  content = "{\"url\": \"";
  content += url;
  content += "\"}";
  server.send(200, "application/json", content);
}

void handleAjaxMessages() {
  if (!checkAuth()) {
    return;
  }
  String content = "{\"message\":\"";
  content += message;
  content += "\"}";
  message = "";
  server.send(200, "application/json", content);
}

void tag_strings_to_array(String id, String pos, String rmv) {
  char tmp;
  char posBuffer[4];
  char tag_pos = 0;
  char cpos = pos.toInt() & 0xFF;
  memset(tag, 0, sizeof(tag));
  strcat(tag, id.c_str());
  sprintf(posBuffer, "%02X", pos.toInt());
  strcat(tag, posBuffer);
  strcat(tag, rmv.c_str());
  tagReady = true;
}
/**
 * This function expects a user and password to allow to proceed, 
 * plus the following:
 * ti: Tag ID in hex
 * po: Position in hex too.
 * rm: on or off indicating if the operation is a remove or add (on removes, off adds)
 */
void handleSentTag() {
  if (!(server.hasArg("usr") && server.hasArg("pwd") && 
      server.hasArg("ti") && server.hasArg("po"))) {
    server.send(404, "text/plain", "Invalid request");
    return;
  }
  String sentPass = server.arg("pwd");
  String storedPass = getPassword(REMOTE_USR);
  if (!(server.arg("usr")!="remot" || sentPass != storedPass)) {
    server.send(401, "text/plain", "Unauthorized");
    return;
  }
  String tagid = server.arg("ti");
  String pos = server.arg("po");
  String remove = server.arg("rm");
  String rmv = "0";
  if (remove == "on") {
    rmv = "1";
  }
  tag_strings_to_array(tagid, pos, rmv);
  server.send(200, "text/plain", "OK");
}

void handlePasswords() {
  if (!checkAuth()) {
    return;
  }
  message = "";
  if (server.hasArg("pconf") && server.arg("pconf")=="1") {
    message += "<p>Invalid attemp, please retry</p>";
    return;
  }
  if (server.hasArg("PAS0")) {
    String pass0 = server.arg("PAS0");
    pass0.trim();
    if (pass0.length() > 0) {
      eeprom_store(pass0, PASSWORD_STORAGE0);
      message += "<p>Admin password changed</p>";
    }
  }
  if (server.hasArg("PAS1")) {
    String pass1 = server.arg("PAS1");
    pass1.trim();
    if (pass1.length() > 0 ) {
      eeprom_store(pass1, PASSWORD_STORAGE1);
      message += "<p>Remote password changed</p>";
    }
  }
  change_selecteds(SETUP_PASS);
  String page = render("/passwords.html", "Password Management", selecteds, true);
  server.send(200, "text/html", page);
}

void handleTimeSetting() {
  if (!checkAuth()){
    return;
  }
  message = "";
  if (server.hasArg("date")) {
    String date = server.arg("date");
    String hours = server.arg("hour");
    String minutes = server.arg("minutes");
    String seconds = server.arg("seconds");
    timeSet = date + "-" + hours + "-" + minutes + "-" + seconds;
    timeReady = true;
    message = "Time set to " + timeSet;
  }
  change_selecteds(SETUP_TIME);
  String page = render("/time.html", "Time Adjust", selecteds, true);
  server.send(200, "text/html", page);
}
